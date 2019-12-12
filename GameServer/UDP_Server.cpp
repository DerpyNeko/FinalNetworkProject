#include "UDP_Server.h"

#include <winsock.h>
#include <WS2tcpip.h>

#include <ctime>
#include <iostream>

#include "FinalProject.pb.h"
using namespace FinalProject;

struct Port {
	unsigned short port;
	struct sockaddr_in si_other;
};

unsigned int numPlayersConnected = 0;
std::vector<Player*> mPlayers;
std::vector<Port*> mPorts;
std::vector<Bullet*> mBullets;

const float UPDATES_PER_SEC = 25; // 5Hz / 200ms per update / 5 updates per second
std::clock_t curr;
std::clock_t prev;
double elapsed_secs;

enum State { ACTIVE, INACTIVE };

void _PrintWSAError(const char* file, int line)
{
	int WSAErrorCode = WSAGetLastError();
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "[WSAError:%d] %S\n", WSAErrorCode, s);
	LocalFree(s);
}

UDPServer::UDPServer(void) : mIsRunning(false), mListenSocket(INVALID_SOCKET)
{
	// WinSock vars
	WSAData		WSAData;
	int			iResult;
	int			iPort = 5150;
	SOCKADDR_IN ReceiverAddr;

	// Step #0 Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0)
	{
		PrintWSAError();
		return;
	}

	// Step #1 Create a socket
	mListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mListenSocket == INVALID_SOCKET)
	{
		PrintWSAError();
		return;
	}

	// Step #2 Bind our socket
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(iPort);
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	iResult = bind(mListenSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	if (iResult == SOCKET_ERROR)
	{
		PrintWSAError();
		return;
	}

	// Set our socket to be nonblocking
	SetNonBlocking(mListenSocket);

	// Our server is ready 
	printf("[SERVER] Receiving IP: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
	printf("[SERVER] Receiving Port: %d\n", htons(ReceiverAddr.sin_port));
	printf("[SERVER] Ready to receive a datagram...\n");

	mIsRunning = true;
	prev = std::clock();

	for (int x = 0; x < 11; x = x + 10)
	{
		for (int z = 0; z < 11; z = z + 10)
		{
			Player* newPlayer = new Player();
			newPlayer->set_state(INACTIVE);
			newPlayer->add_position(x);
			newPlayer->add_position(z);
			newPlayer->add_velocity(0);
			newPlayer->set_orientation(0);
			newPlayer->set_isshooting(false);
			mPlayers.push_back(newPlayer);

			Port* newPort;
			mPorts.push_back(newPort);

			Bullet* newBullet = new Bullet();
			newBullet->set_state(INACTIVE);
			newBullet->add_position(x);
			newBullet->add_position(z);
			newBullet->add_velocity(0);
			newBullet->set_starttime(0.0f);
			newBullet->set_orientation(0);
			mBullets.push_back(newBullet);
		}
	}
}

UDPServer::~UDPServer(void)
{
	closesocket(mListenSocket);
	WSACleanup();	// <-- Not necessary if quitting application, Windows will handle this.
}

void UDPServer::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		PrintWSAError();
		return;
	}
}

void UDPServer::Update(void)
{
	if (!mIsRunning)
		return;

	ReadData();

	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);

	if (elapsed_secs < (1.0f / UPDATES_PER_SEC))
		return;

	prev = curr;

	UpdatePlayers();
	UpdateBullets();
	BroadcastUpdate();
}

void UDPServer::UpdatePlayers(void)
{
	for (int i = 0; i < numPlayersConnected; i++)
	{
		float orientation = mPlayers[i]->orientation();
		float currentx = mPlayers[i]->position(0);
		float currentz = mPlayers[i]->position(1);

		if (currentz < 10.25f)
			if (orientation == 1)
				currentz += 0.05f;

		if (currentz > -0.25f)
			if (orientation == 2)
				currentz -= 0.05f;

		if (currentx < 10.25f)
			if (orientation == 3)
				currentx += 0.05f;

		if (currentx < 10.25f)
			if (orientation == 4)
				currentx -= 0.05f;

		if (orientation != 0)
		{
			if (mPlayers[i]->isshooting())
			{
				if (mBullets[i]->state() == INACTIVE)
				{
					std::cout << "SPACE HAS BEEN PRESSED" << std::endl;
					mBullets[i]->set_state(ACTIVE);
					mBullets[i]->set_starttime(std::clock());
					mBullets[i]->set_orientation(orientation);
				}
			}
		}

		mPlayers[i]->set_position(0, currentx);
		mPlayers[i]->set_position(1, currentz);

		std::cout << "Player " << i << " position: " << mPlayers[i]->position(0) << ", " << mPlayers[i]->position(1) << std::endl;
	}
}

void UDPServer::UpdateBullets(void)
{
	for (int i = 0; i < numPlayersConnected; i++)
	{
		float orientation = mBullets[i]->orientation();
		float currentx = mBullets[i]->position(0);
		float currentz = mBullets[i]->position(1);

		float currentTime = std::clock();

		if (mBullets[i]->state() == ACTIVE)
		{
			std::cout << "BULLET IS NOW ACTIVE" << std::endl;
			std::cout << "Current time: " << currentTime << "\nStart time: " << mBullets[i]->starttime() << std::endl;
			std::cout << "MATH: " << (currentTime - mBullets[i]->starttime()) / (double)CLOCKS_PER_SEC << std::endl;
			// THIS MATH IS QUESTIONABLE
			if ((currentTime - mBullets[i]->starttime()) / (double)CLOCKS_PER_SEC < 2.0f)
			{
				// DO COLLISION CHECK HERE 
				std::cout << "ORIENTATION: " << orientation << std::endl;

				// IF NOT COLLISION, MOVE BULLET
				if (orientation == 1)
					currentz += 0.08f;

				if (orientation == 2)
					currentz -= 0.08f;

				if (orientation == 3)
					currentx += 0.08f;

				if (orientation == 4)
					currentx -= 0.08f;

				mBullets[i]->set_position(0, currentx);
				mBullets[i]->set_position(1, currentz);
			}
			else
			{
				mBullets[i]->set_state(INACTIVE);
			}
		}
		else
		{
			mBullets[i]->set_position(0, mPlayers[i]->position(0));
			mBullets[i]->set_position(1, mPlayers[i]->position(1));
		}

		std::cout << "Bullet " << i << " position: " << mBullets[i]->position(0) << ", " << mBullets[i]->position(1) << std::endl;
	}
}

void UDPServer::BroadcastUpdate(void)
{
	GameScene* scene = new GameScene();
	scene->set_id(0);

	for (Player* p : mPlayers)
	{
		Player* player = scene->add_players();
		player->set_state(p->state());
		player->add_position(p->position(0));
		player->add_position(p->position(1));
		player->add_velocity(p->velocity(0));
		player->set_orientation(p->orientation());
		player->set_isshooting(p->isshooting());
	}

	for (Bullet* b : mBullets)
	{
		Bullet* bullet = scene->add_bullets();
		bullet->set_state(b->state());
		bullet->add_position(b->position(0));
		bullet->add_position(b->position(1));
		bullet->add_velocity(b->velocity(0));
		bullet->set_orientation(b->orientation());
		bullet->set_starttime(b->starttime());
	}

	std::string serializedResult = scene->SerializeAsString();

	std::vector<char> packet;
	packet.push_back(0);
	packet.push_back(serializedResult.length());
	std::cout << "NEW LENGTH: " << serializedResult.length() << std::endl;

	const char* temp = serializedResult.c_str();
	for (int i = 0; i < serializedResult.length(); i++)
	{
		packet.push_back(temp[i]);
	}

	unsigned int id = packet[0];
	unsigned char a = packet[1];
	unsigned int la = a;

	std::cout << "id: " << id << " SIZE: " << la << std::endl;

	for (int i = 0; i < 4; i++)
	{
		int result = sendto(mListenSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & (mPorts[i]->si_other), sizeof(mPorts[i]->si_other));

		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return;

			std::cout << "Erroring in send" << std::endl;
			PrintWSAError();
			return;
		}

		if (result == 0)
		{
			printf("Disconnected...\n");
			return;
		}
	}
}

Player* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other, SOCKET mListenSocket)
{
	// If a player with this port is already connected, return it
	for (int i = 0; i < mPlayers.size(); i++)
	{
		if (mPorts[i]->port == port)
		{
			std::cout << "Port: " << mPorts[numPlayersConnected]->port << std::endl;
			std::cout << "Port " << i << ": " << mPorts[i]->port << std::endl;
			std::cout << "Found existing port" << std::endl;
			return mPlayers[i];
		}
	}

	// Otherwise create a new player, and return that one!
	mPorts[numPlayersConnected]->port = port;
	mPorts[numPlayersConnected]->si_other = si_other;
	mPlayers[numPlayersConnected]->set_state(ACTIVE);
	mBullets[numPlayersConnected]->set_state(INACTIVE);

	std::cout << "Created player port" << std::endl;

	PlayerNumber* playerNum = new PlayerNumber();
	playerNum->set_number(-1);
	std::cout << "Player num from packet is: " << playerNum->number() << std::endl;
	playerNum->set_number(numPlayersConnected);
	std::string serializedResult = playerNum->SerializeAsString();

	std::cout << "Number is: " << playerNum->number() << std::endl;

	std::vector<char> packet;
	packet.push_back(10);
	packet.push_back(serializedResult.length());

	const char* temp = serializedResult.c_str();
	for (int i = 0; i < serializedResult.length(); i++)
	{
		packet.push_back(temp[i]);
	}

	int result = sendto(mListenSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & si_other, sizeof(si_other));

	return mPlayers[numPlayersConnected++];
}

void UDPServer::ReadData(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	std::vector<char> packet(512);

	int result = recvfrom(mListenSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return;

		PrintWSAError();

		std::cout << "Casually erroring" << std::endl;
		packet.clear();
		return;
	}

	unsigned short port = si_other.sin_port;

	unsigned int id = packet[0];
	unsigned int length = packet[1];
	std::string packetContents;

	for (int i = 2; i <= length + 1; i++)
	{
		packetContents += packet[i];
	}

	if (id == 10)
	{
		//PlayerNumber* playerNum = new PlayerNumber();
		//playerNum->ParseFromString(packetContents);
		//if (playerNum->number() == -1)
		//{
		//	playerNum->set_number(numPlayersConnected--);
		//	std::string serializedResult = playerNum->SerializeAsString();

		//	std::vector<char> packet;
		//	packet.push_back(id);
		//	packet.push_back(serializedResult.length());

		//	const char* temp = serializedResult.c_str();
		//	for (int i = 0; i < serializedResult.length(); i++)
		//	{
		//		packet.push_back(temp[i]);
		//	}

		//	int result = sendto(mListenSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & si_other, sizeof(si_other));
		//}

	}
	else
	{
		UserInput* input = new UserInput();
		input->ParseFromString(packetContents);

		Player* player = GetPlayerByPort(port, si_other, mListenSocket);

		//player->up = buffer[0] == 1;
		//player->down = buffer[1] == 1;
		//player->right = buffer[2] == 1;
		//player->left = buffer[3] == 1;

		player->set_orientation(input->input());
		player->set_isshooting(input->isshooting());

		//printf("%d : %hu received { %d %d %d %d }\n", mListenSocket, port, player->up, player->down, player->right, player->left);

		std::cout << "Player input is " << player->orientation() << std::endl;

		// Send the data back to the client
		// result = sendto(mListenSocket, buffer, 1, 0, (struct sockaddr*) & si_other, sizeof(si_other));
	}
}

