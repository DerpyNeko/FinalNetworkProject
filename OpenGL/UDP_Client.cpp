#include "udp_client.h"

#include <bitset>
#include <iostream>

struct Bullet {
	float x, z;
};

struct Player {
	float x, z;
	bool isShootingBullet;
	Bullet bullet;
};

std::vector<Player> mPlayers;

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
	x = mPlayers[id].x;
	z = mPlayers[id].z;
}

UDPClient::UDPClient(void)
	: mServerSocket(INVALID_SOCKET)
{
	mPlayers.resize(4);

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
	char buffer[512];

	int result = recvfrom(mServerSocket, buffer, 512, 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return;
		}
		std::cout << "Erroring in recv" << std::endl;
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', 512);
		return;
	}

	// NumPlayers
	// Each player: { x, y }
	unsigned int numPlayers;
	memcpy(&numPlayers, &(buffer[0]), sizeof(unsigned int));

	float x, z;
	for (int i = 0; i < numPlayers; i++) {
		memcpy(&x, &(buffer[i * 8 + 4]), sizeof(float));
		memcpy(&z, &(buffer[i * 8 + 8]), sizeof(float));
		mPlayers[i].x = x;
		mPlayers[i].z = z;
	}

	//unsigned short port = si_other.sin_port;
	//printf("%d : %hu received %d bytes\n", mServerSocket, port, result);

	printf("%d players: {", numPlayers);
	for (int i = 0; i < numPlayers; i++) {
		printf(" {x: %.2f, z: %.2f}", mPlayers[i].x, mPlayers[i].z);
	}
	printf(" }\n");
}

void UDPClient::Send(char* data, int numBytes)
{
	int result = sendto(mServerSocket, data, numBytes, 0,
		(struct sockaddr*) & si_other, sizeof(si_other));

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
