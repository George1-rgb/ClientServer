#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include "Server.h"
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	Server server;
	server.StartServer("192.168.0.57", 1111);
	
	return 0;
}