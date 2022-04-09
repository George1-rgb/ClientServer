#include "Server.h"
#include <algorithm>
#include "Global.h"
Server::Server()
{
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) 
	{
		Global::instance().getLogger().log(Message, "Server.cpp", "Error WinSock version initializaion #" + WSAGetLastError());
		exit(-1);
	}
	ServSock = socket(AF_INET, SOCK_STREAM, NULL);
	if (ServSock == INVALID_SOCKET) 
	{
		Global::instance().getLogger().log(Message, "Server.cpp", "Error initialization socket # " + WSAGetLastError());
		closesocket(ServSock);
		exit(-1);
	}

}

Server::~Server()
{
	Global::instance().getLogger().log(Message, "Server.cpp", "Server stopped.");
}

void Server::StartServer(string IP, int PORT)
{
	in_addr ip_to_num;
	if (inet_pton(AF_INET, IP.c_str(), &ip_to_num) <= 0)
	{
		Global::instance().getLogger().log(Message, "Server.cpp", "Error in IP translation to special numeric format");
		exit(-1);
	}
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT);
	if (bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo)) != 0)
	{
		Global::instance().getLogger().log(Message, "Server.cpp", "Error Socket binding to server info. Error # " + WSAGetLastError());
		closesocket(ServSock);
		exit(-1);
	}
	else
	{
		cout << "Server started with IP: " << IP << " and PORT: " << PORT << endl;
		Global::instance().getLogger().log(Message, "Server.cpp", "Server started with IP: " + IP + " and PORT: " + to_string(PORT));
	}
	if (listen(ServSock, SOMAXCONN) != 0)
	{
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
	}
	ConnectingClients(servInfo);
}

void Server::ConnectingClients(sockaddr_in addr)
{
	int clientInfo_size = sizeof(addr);
	SOCKET ClientConn;
	bool isEnter = false;
	for (int i = 0; i < SOMAXCONN; i++)
	{
		ClientConn = accept(ServSock, (sockaddr*)&addr, &clientInfo_size);
		//ѕровер€ем есть ли до этого свободные места    
		for (int j = 0; j < connections.size(); j++)
		{
			if (connections[j] == NULL)
			{
				i--;
				cout << "Connected \tID " << j << endl;
				connections[j] = ClientConn;
				isEnter = true;
				thread th([this, j]()
					{
						ClientHandler(j);
					});
				th.detach();
				this_thread::sleep_for(chrono::milliseconds(10));
				break;
			}
		}

		if (isEnter)
		{
			isEnter = false;
			continue;
		}
		else
		{
			connections.push_back(ClientConn);
			cout << "Connected \tID " << i << endl;
			thread th([this, i]()
				{
					ClientHandler(i);
				});
			th.detach();
			this_thread::sleep_for(chrono::milliseconds(10));
		}
	}
}

bool Server::ProcessPacket(int index, Packet packetType)
{	
	switch (packetType)
	{
	case P_ChatMessage:
	{
		int msg_size;
		if (recv(connections[index], (char*)&msg_size, sizeof(int), NULL) == SOCKET_ERROR) closesocket(connections[index]);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		if (recv(connections[index], msg, msg_size, NULL) == SOCKET_ERROR) closesocket(connections[index]);

		for (auto conn : connections)
		{
			if (conn == connections[index] || conn == NULL) continue;
			Packet packetType = P_ChatMessage;
			if (send(conn, (char*)&packetType, sizeof(Packet), NULL) == SOCKET_ERROR) closesocket(connections[index]);
			if (send(conn, (char*)&msg_size, sizeof(int), NULL) == SOCKET_ERROR) closesocket(connections[index]);
			if (send(conn, msg, msg_size, NULL) == SOCKET_ERROR) closesocket(connections[index]);
		}
		delete[] msg;
	}
	break;
	case P_Authorize:
	{
		bool isAuthorization = true;
		int msg_size;
		if (recv(connections[index], (char*)&msg_size, sizeof(int), NULL) == SOCKET_ERROR) closesocket(connections[index]);
		char* name = new char[msg_size + 1];
		name[msg_size] = '\0';
		if (recv(connections[index], name, msg_size, NULL) == SOCKET_ERROR) closesocket(connections[index]);
		send(connections[index], (char*)&isAuthorization, sizeof(bool), NULL);
	}
	break;
	case P_Close:
	{
		cout << "Disconnected \tID " << index << endl;
		return false;
	}
	break;
	default:
	{
		cout << "Unrecognized packet: " << packetType << endl;
		return false;
	}
		break;
	}
	return true;
}

void Server::ClientHandler(int index)
{
	Packet packetType;
	while (true)
	{
		recv(connections[index], (char*)&packetType, sizeof(Packet), NULL);
		if (!ProcessPacket(index, packetType)) break;
	}
	Disconnect(connections[index]);
	return;
}

void Server::Disconnect(SOCKET& curSock)
{
	Packet packetType = P_Close;
	send(curSock, (char*)&packetType, sizeof(Packet), NULL);
	closesocket(curSock);
	curSock = 0;
}