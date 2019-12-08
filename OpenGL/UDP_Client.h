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

//void _PrintWSAError(const char* file, int line);
//#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)
