#include "udp_client.h"

#include <bitset>
#include <iostream>

#include "FinalProject.pb.h"
using namespace FinalProject;

//struct Bullet {
//	float x, z;
//};

//struct Player {
//	float x, z;
//	bool isShootingBullet;
//	Bullet bullet;
//};

std::vector<Player*> mPlayers;

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

void UDPClient::SetPosition(int id, float& x, float& z)
{
	std::cout << "PositionID: " << id << std::endl;
	x = mPlayers[id]->position(0);
	z = mPlayers[id]->position(1);
}

UDPClient::UDPClient(void) :mServerSocket(INVALID_SOCKET)
{
	//mPlayers.resize(4);

	WSAData		WSAData;
	int			iResult;
	int			Port = 5150;
	SOCKADDR_IN ReceiverAddr;

	// Step #0 Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0) {
		std::cout << "Erroring in initalization" << std::endl;
		PrintWSAError();
		return;
	}

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
	if (result == SOCKET_ERROR) {
		std::cout << "Erroring in set non blocking" << std::endl;
		PrintWSAError();
		return;
	}
}

void UDPClient::CreateSocket(string ip, int port)
{
	mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == SOCKET_ERROR) {
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
	//char buffer[512];
	std::vector<char> packet(512);

	int result = recvfrom(mServerSocket, &packet[0], packet.size(), 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return;
		}
		std::cout << "Erroring in recv" << std::endl;
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		//memset(buffer, '\0', 512);
		packet.clear();
		return;
	}

	// NumPlayers
	// Each player: { x, y }
	unsigned int numPlayers = 1;
	//memcpy(&numPlayers, &(buffer[0]), sizeof(unsigned int));

	int length = packet[0];
	std::string packetContents;

	for (int i = 1; i <= length + 1; i++)
	{
		packetContents += packet[i];
	}

	GameScene* scene = new GameScene();
	scene->ParseFromString(packetContents);

	float x, z;
	for (int i = 0; i < numPlayers; i++) {
		
		mPlayers[i]->set_position(0, scene->players(i).position(0));
		mPlayers[i]->set_position(1, scene->players(i).position(1));
	}

	//unsigned short port = si_other.sin_port;
	//printf("%d : %hu received %d bytes\n", mServerSocket, port, result);

	printf("%d players: {", numPlayers);
	for (int i = 0; i < numPlayers; i++) {
		printf(" {x: %.2f, z: %.2f}", mPlayers[i]->position(0), mPlayers[i]->position(1));
	}
	printf(" }\n");
}

void UDPClient::SendInput(int direction)
{
	UserInput* input = new UserInput();
	input->set_id(0);
	input->set_input(direction);
	std::string serializedResult = input->SerializeAsString();
	Send(serializedResult);
}

void UDPClient::Send(std::string serializedString)
{
	// Packet -> [contentSize][content]
	std::vector<char> packet;
	//packet.push_back(id);
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
		if (WSAGetLastError() == WSAEWOULDBLOCK) return;
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
