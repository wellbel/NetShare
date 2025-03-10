#if !defined(__APPLE__) && !defined(__linux__)

#ifndef NSHW32
#define NSHW32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define DEFAULT_PORT "21984"
#define DEFAULT_BUFLEN 2048

#define _WIN32_WINNT 0x501 // freeaddrinfo not declared errors

#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

using std::cout;
using std::string;

bool setOutputFile(const string&, const string&, std::ofstream&);
int main(int, char**);

#endif
#endif