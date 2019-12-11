#include "UDP_Server.h"

#include <winsock.h>
#include <WS2tcpip.h>

#include <ctime>
#include <iostream>

#include "FinalProject.pb.h"
using namespace FinalProject;

//struct Player {
//	unsigned short port; // their id;
//	struct sockaddr_in si_other;
//	float x;
//	float z;
//	bool up, down, right, left;
//};

struct Port {
	unsigned short port;
	struct sockaddr_in si_other;
};

unsigned int numPlayersConnected = 0;

std::vector<Player*> mPlayers;
std::vector<Port*> mPorts;

const float UPDATES_PER_SEC = 5;		// 5Hz / 200ms per update / 5 updates per second
std::clock_t curr;
std::clock_t prev;
double elapsed_secs;


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

UDPServer::UDPServer(void)
	: mIsRunning(false)
	, mListenSocket(INVALID_SOCKET)
	, mAcceptSocket(INVALID_SOCKET)
{
	//mPlayers.resize(4);

	// WinSock vars
	WSAData		WSAData;
	int			iResult;
	int			port = 5150;
	SOCKADDR_IN ReceiverAddr;

	// Step #0 Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0) {
		PrintWSAError();
		return;
	}

	// Step #1 Create a socket
	mListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mListenSocket == INVALID_SOCKET) {
		PrintWSAError();
		return;
	}

	// Step #2 Bind our socket
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(port);
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	iResult = bind(mListenSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	if (iResult == SOCKET_ERROR) {
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
			newPlayer->set_state(0);
			newPlayer->add_position(x);
			newPlayer->add_position(z);
			newPlayer->add_velocity(0);
			newPlayer->add_orientation(0);

			mPlayers.push_back(newPlayer);

			Port* newPort;

			mPorts.push_back(newPort);
		}
	}

} // end UDPServer

UDPServer::~UDPServer(void)
{
	closesocket(mListenSocket);
	WSACleanup();	// <-- Not necessary if quitting application, Windows will handle this.
}

void UDPServer::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}
}

void UDPServer::Update(void)
{
	if (!mIsRunning) return;

	// TODO: ReadData, SendData
	std::cout << "READ DATA" << std::endl;
	ReadData();

	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);

	if (elapsed_secs < (1.0f / UPDATES_PER_SEC)) return;
	prev = curr;

	std::cout << "UPDATE PLAYERS" << std::endl;
	UpdatePlayers();
	std::cout << "BROADCAST" << std::endl;
	BroadcastUpdate();
}

void UDPServer::UpdatePlayers(void)
{
	for (int i = 0; i < numPlayersConnected; i++) {
		//if (mPlayers[i].up) mPlayers[i].z += 10.0f * elapsed_secs;
		//if (mPlayers[i].down) mPlayers[i].z -= 10.0f * elapsed_secs;
		//if (mPlayers[i].right) mPlayers[i].x += 10.0f * elapsed_secs;
		//if (mPlayers[i].left) mPlayers[i].x -= 10.0f * elapsed_secs;
		//TODO: Fix this speed
		float orientation = mPlayers[i]->orientation(0);
		float currentx = mPlayers[i]->position(0);
		float currentz = mPlayers[i]->position(1);

		if (orientation == 0) currentz += 0.05f;
		if (orientation == 1) currentz -= 0.05f;
		if (orientation == 2) currentx += 0.05f;
		if (orientation == 3) currentx -= 0.05f;
		
		mPlayers[i]->set_position(0, currentx);
		mPlayers[i]->set_position(1, currentz);
		
		std::cout << mPlayers[i]->position(0) << ", " << mPlayers[i]->position(1) << std::endl;
	}
}

void UDPServer::BroadcastUpdate(void)
{
	// create our data to send, then send the same data to all players
	//const int DEFAULT_BUFLEN = 512;
	//char buffer[512];
	//memset(buffer, '\0', DEFAULT_BUFLEN);

	//memcpy(&(buffer[0]), &numPlayersConnected, sizeof(unsigned int));

	//for (int i = 0; i < numPlayersConnected; i++) {
	//	float x = mPlayers[i].x;
	//	float y = mPlayers[i].z;
	//	memcpy(&(buffer[i * 8 + 4]), &x, sizeof(float));
	//	memcpy(&(buffer[i * 8 + 8]), &y, sizeof(float));
	//}

	//int result = sendto(mListenSocket, buffer, 12, 0,
	//	(struct sockaddr*) & (mPlayers[0].si_other), sizeof(mPlayers[0].si_other));

	GameScene* scene = new GameScene();
	for (Player* i : mPlayers)
	{
		Player* p = scene->add_players();
		p->set_state(i->state());
		p->add_position(i->position(0));
		p->add_position(i->position(1));
		p->add_velocity(i->velocity(0));
		p->add_orientation(i->orientation(0));
	}

	std::string serializedResult = scene->SerializeAsString();

	std::vector<char> packet;
	packet.push_back(serializedResult.length());

	const char* temp = serializedResult.c_str();
	for (int i = 0; i < serializedResult.length(); i++)
	{
		packet.push_back(temp[i]);
	}

	int result = sendto(mListenSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & (mPorts[0]->si_other), sizeof(mPorts[0]->si_other));

	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) return;
		std::cout << "Erroring in send" << std::endl;
		PrintWSAError();
		return;
	}

	if (result == 0) {
		printf("Disconnected...\n");
		return;
	}
}

Player* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
{
	// If a player with this port is already connected, return it
	for (int i = 0; i < mPlayers.size(); i++) {
		if (mPorts[i]->port == port)
		{
			std::cout << "Is doing this thing" << std::endl;
			return mPlayers[i];
		}
	}

	// Otherwise create a new player, and return that one!
	mPorts[numPlayersConnected]->port = port;
	mPlayers[numPlayersConnected]->set_position(0, 0.0f);
	mPlayers[numPlayersConnected]->set_position(1, 0.0f);
	mPorts[numPlayersConnected]->si_other = si_other;
	std::cout << "Created new player?" << std::endl;
	return mPlayers[numPlayersConnected++];
}

void UDPServer::ReadData(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	//char buffer[512];
	std::vector<char> packet(512);

	int result = recvfrom(mListenSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			std::cout << "Skip read" << std::endl;
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		//memset(buffer, '\0', 512);
		std::cout << "Casually erroring" << std::endl;
		packet.clear();
		return;
	}

	unsigned short port = si_other.sin_port;

	int length = packet[0];
	std::string packetContents;

	for (int i = 1; i <= length + 1; i++)
	{
		packetContents += packet[i];
	}

	UserInput* input = new UserInput();
	input->ParseFromString(packetContents);

	Player* player = GetPlayerByPort(port, si_other);

	//player->up = buffer[0] == 1;
	//player->down = buffer[1] == 1;
	//player->right = buffer[2] == 1;
	//player->left = buffer[3] == 1;

	player->set_orientation(0, input->input());

	//printf("%d : %hu received { %d %d %d %d }\n", mListenSocket, port, player->up, player->down, player->right, player->left);

	std::cout << "Player input is " << input->input() << std::endl;

	// Send the data back to the client
	// result = sendto(mListenSocket, buffer, 1, 0, (struct sockaddr*) & si_other, sizeof(si_other));
}

