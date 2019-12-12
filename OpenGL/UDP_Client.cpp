#include "udp_client.h"

#include <bitset>
#include <iostream>

#include "FinalProject.pb.h"
using namespace FinalProject;

int mPlayerNumber = -1;

std::vector<Player*> mPlayers;
std::vector<Bullet*> mBullets;

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

void UDPClient::SetPlayerNumber(int& num)
{
	std::cout << "NUM IS " << num << " mPlayerNumber is: " << mPlayerNumber << std::endl;
	num = mPlayerNumber;
}

void UDPClient::SetPlayerPosition(int id, float& x, float& z)
{
	x = mPlayers[id]->position(0);
	z = mPlayers[id]->position(1);
}

void UDPClient::SetBulletPosition(int id, float& x, float& z)
{
	x = mBullets[id]->position(0);
	z = mBullets[id]->position(1);
}

UDPClient::UDPClient(void) :mServerSocket(INVALID_SOCKET)
{
	WSAData		WSAData;
	int			iResult;

	// Step #0 Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0) 
	{
		std::cout << "Erroring in initalization" << std::endl;
		PrintWSAError();
		return;
	}

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

			Bullet* newBullet = new Bullet();
			newBullet->set_state(INACTIVE);
			newBullet->add_position(x);
			newBullet->add_position(z);
			newBullet->add_velocity(0);
			newBullet->set_starttime(0.0f);
			mBullets.push_back(newBullet);
		}
	}
}

UDPClient::~UDPClient(void)
{
	closesocket(mServerSocket);
	WSACleanup();
}

void UDPClient::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) 
	{
		std::cout << "Erroring in set non blocking" << std::endl;
		PrintWSAError();
		return;
	}
}

void UDPClient::CreateSocket(string ip, int port)
{
	mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == SOCKET_ERROR) 
	{
		std::cout << "Erroring in creating socket" << std::endl;
		PrintWSAError();
		return;
	}

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	si_other.sin_addr.s_addr = inet_addr(ip.c_str());

	SetNonBlocking(mServerSocket);
}

void UDPClient::Update(void)
{
	Recv();
}

void UDPClient::Recv(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	std::vector<char> packet(512);

	int result = recvfrom(mServerSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return;
		
		std::cout << "Erroring in recv" << std::endl;
		PrintWSAError();

		packet.clear();
		return;
	}
	
	//if (mPlayerNumber == -1)
	//{
	//	PlayerNumber* playerNum = new PlayerNumber();
	//	playerNum->set_number(-1);
	//	std::string serializedResult = playerNum->SerializeAsString();
	//	Send(10, serializedResult);
	//}

	// IMPORTANT
	unsigned int numPlayers = 4;

	std::string packetContents;
	unsigned int id = packet[0];
	unsigned char a = packet[1];
	unsigned int length = a;

	for (int i = 2; i <= length + 1; i++)
	{
		packetContents += packet[i];
	}

	if (id == 10)
	{
		PlayerNumber* playerNum = new PlayerNumber();
		playerNum->ParseFromString(packetContents);
		mPlayerNumber = playerNum->number();
	}
	else
	{
		GameScene* scene = new GameScene();
		scene->ParseFromString(packetContents);

		float x, z;
		for (int i = 0; i < numPlayers; i++)
		{
			mPlayers[i]->set_state(scene->players(i).state());
			mPlayers[i]->set_position(0, scene->players(i).position(0));
			mPlayers[i]->set_position(1, scene->players(i).position(1));

			mBullets[i]->set_state(scene->bullets(i).state());
			mBullets[i]->set_position(0, scene->bullets(i).position(0));
			mBullets[i]->set_position(1, scene->bullets(i).position(1));
		}

		//unsigned short port = si_other.sin_port;
		//printf("%d : %hu received %d bytes\n", mServerSocket, port, result);

		printf("%d players: {", numPlayers);
		for (int i = 0; i < numPlayers; i++)
		{
			printf(" {x: %.2f, z: %.2f}", mPlayers[i]->position(0), mPlayers[i]->position(1));
		}
		printf(" }\n");
	}
}

void UDPClient::SendInput(int command, bool isShooting)
{
	UserInput* input = new UserInput();
	input->set_id(0);
	input->set_input(command);
	input->set_isshooting(isShooting);
	std::string serializedResult = input->SerializeAsString();
	Send(0, serializedResult);
}

void UDPClient::Send(int id, std::string serializedString)
{
	// Packet -> [id][contentSize][content]
	std::vector<char> packet;
	packet.push_back(id);
	packet.push_back(serializedString.length());
	
	const char* temp = serializedString.c_str();
	for (int i = 0; i < serializedString.length(); i++)
	{
		packet.push_back(temp[i]);
	}
	
	int result = sendto(mServerSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & si_other, sizeof(si_other));
	//	Sleep(10);
//	int result = sendto(mServerSocket, data, numBytes, 0, (struct sockaddr*) & si_other, sizeof(si_other));

	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) 
			return;

		std::cout << "Erroring in send" << std::endl;
		PrintWSAError();
		return;
	}

	if (result == 0) {
		printf("Disconnected...\n");
		return;
	}

	// printf("Number of bytes sent: %d\n", result);
}

// Sends message to client
//void SendToClient(SOCKET connection, int id, std::string serializedString)
//{
//	// Packet -> [requestId][contentSize][content]
//	std::vector<char> packet;
//	packet.push_back(id);
//	packet.push_back(serializedString.length());
//
//	const char* temp = serializedString.c_str();
//	for (int i = 0; i < serializedString.length(); i++)
//	{
//		packet.push_back(temp[i]);
//	}
//
//	send(connection, &packet[0], packet.size(), 0);
//	Sleep(10);
//}
