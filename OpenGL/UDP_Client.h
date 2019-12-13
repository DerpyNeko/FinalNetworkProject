#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include <vector>
#include <string>

using std::string;

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

class UDPClient
{
public:
	UDPClient(void);
	~UDPClient(void);

	void CreateSocket(string ip, int port);
	void Update(void);

	void SendInput(int command, bool isShooting);
	void Send(int id, std::string serializedString);

	void SetPlayerNumber(int& num);
	void SetPlayerPosition(int id, float& x, float& z, int& rotation, bool& isActive);
	void SetBulletPosition(int id, float& x, float& z);
	void SetHitboxPosition(int id, float& x, float& z);
private:
	void SetNonBlocking(SOCKET socket);
	void Recv(void);

	SOCKET mServerSocket;
	struct sockaddr_in si_other;
};