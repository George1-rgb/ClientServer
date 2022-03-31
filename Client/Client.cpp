#include "Client.h"
#include <iostream>
#include "Types.h"
using namespace std;
Client::Client()
{
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != NULL) 
	{
		cout << "Error WinSock version initializaion #" << WSAGetLastError() << endl;
		exit(-1);
	}
	ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	CheckSocket();
}

Client::~Client()
{
	Close();
}

void Client::Connect(string IP, int PORT)
{
	in_addr ip_to_num;
	inet_pton(AF_INET, IP.c_str(), &ip_to_num);
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT);
	if (connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo)) != NULL)
	{
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		//exit(-1);
	}
	else
		cout << "***Connection to server SUCCESSFULLY!***" 
		<< "\n!!!BEFORE CLOSING THE APPLICATION, BE SURE TO DISSCONNECT FROM THE SERVER!!!\n***To disconnect, enter 'XXX'***" << endl;
	thread th([this]() 
	{
			ClientHandler();
	});
	th.detach();
	SendingMess();
}

void Client::CheckSocket()
{
	if (ClientSock == INVALID_SOCKET)
	{
		cout << "SOCKETERROR " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(-1);
	}
}

void Client::SendingMess()
{
	short packet_size = 0;
	string msg;												// The size of sending / receiving packet in bytes
	while (true)
	{
		getline(cin, msg);
		if (msg == "XXX")
		{
			Close();
			break;
		}
		else
		{ 
			Packet packetType = P_ChatMessage;
			send(ClientSock, (char*)&packetType, sizeof(Packet), NULL);
			int msg_size = msg.size();
			send(ClientSock, (char*)&msg_size, sizeof(int), NULL);
			send(ClientSock, msg.c_str(), msg_size, NULL);
			Sleep(10);
		}
	}
}

void Client::ClientHandler()
{
	Packet packetType;
	while (true)
	{
		recv(ClientSock, (char*)&packetType, sizeof(Packet), NULL);
		if (!ProcessPacket(packetType)) break;
	}
	return;
}

bool Client::ProcessPacket(Packet packetType)
{
	switch (packetType)
	{
	case P_ChatMessage:
	{
		int msg_size;
		short packet_size = 0;
		packet_size = recv(ClientSock, (char*)&msg_size, sizeof(int), NULL);
		if (packet_size == SOCKET_ERROR)
		{
			cout << "Can't recieve a massage from server\n";
			closesocket(ClientSock);
			WSACleanup();
		}
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		packet_size = recv(ClientSock, msg, msg_size, NULL);
		if (packet_size == SOCKET_ERROR)
		{
			cout << "Can't recieve a massage from server\n";
			closesocket(ClientSock);
			WSACleanup();
		}
		cout << msg << endl;
		delete[] msg; 
	}
	break;
	case P_Close:
	{
		closesocket(ClientSock);
		cout << "***DISCONNECTED***\n***Now you can close the window***" << endl;
		return false;
	}
	break;
	default:
	{
		cout << "Unrecognized packet: " << packetType << endl;
		break;
	}
	}
	return true;
}

void Client::Close()
{
	Packet packetType = P_Close;
	send(ClientSock, (char*)&packetType, sizeof(Packet), NULL);
}

