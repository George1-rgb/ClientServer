#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "Types.h"
#include <thread>
#include <mutex>
using namespace std;
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
class Client
{
public:
	Client();
	~Client();
	void Connect(string IP, int PORT);
	void ClientHandler();
private:
	SOCKET ClientSock;
	string userName;
	bool Authorization();
	void CheckSocket();
	void SendingMess();
	bool ProcessPacket(Packet packetType);
	void Close();
};

