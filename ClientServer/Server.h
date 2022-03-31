#pragma once
#include "Types.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <mutex>
#include <list>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
class Server
{
public:
	Server();
	~Server();
	void StartServer(string IP, int PORT);
private:
	bool ProcessPacket(int index, Packet packetType);
	void ConnectingClients(sockaddr_in addr);
	void Disconnect(SOCKET &curSock);
	void ClientHandler(int index);
	vector<SOCKET> connections;
	SOCKET ServSock;
};

