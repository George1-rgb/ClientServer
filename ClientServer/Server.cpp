#include "Server.h"
#include <algorithm>
#include <mutex>
Server::Server()
{
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) 
	{
		cout << "Error WinSock version initializaion #" << WSAGetLastError() << endl;
		exit(-1);
	}
	ServSock = socket(AF_INET, SOCK_STREAM, NULL);
	if (ServSock == INVALID_SOCKET) 
	{
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		exit(-1);
	}

}

void Server::StartServer(string IP, int PORT)
{
	in_addr ip_to_num;
	if (inet_pton(AF_INET, IP.c_str(), &ip_to_num) <= 0)
	{
		cout << "Error in IP translation to special numeric format" << endl;
		exit(-1);
	}
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT);
	if (bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo)) != 0)
	{
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		exit(-1);
	}
	else
		cout << "Server started with IP: " << IP << " and PORT: " << PORT << endl;

	if (listen(ServSock, SOMAXCONN) != 0)
	{
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
	}
	else 
	{
		cout << "Listening..." << endl;
	}

	int clientInfo_size = sizeof(servInfo);
	SOCKET ClientConn;
	sockaddr_in clientInfo;
	for (int i = 0; i < SOMAXCONN; i++)
	{
		ClientConn = accept(ServSock, (sockaddr*)&servInfo, &clientInfo_size);
		if (ClientConn == NULL)
		{
			closesocket(ClientConn);
			continue;
		}
		else
		{
			connections.push_back(ClientConn);
			cout << "Current number of connections " << connections.size() << endl;
			cout << "Client has connected!" << endl;
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
	if (index >= connections.size())
		index--;
	
	cout << "Current index" << index << endl;
	switch (packetType)
	{
	case P_ChatMessage:
	{
		cout << "Message" << endl;
		int msg_size;
		if (recv(connections[index], (char*)&msg_size, sizeof(int), NULL) == SOCKET_ERROR) closesocket(connections[index]);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		if (recv(connections[index], msg, msg_size, NULL) == SOCKET_ERROR) closesocket(connections[index]);

		for (auto conn : connections)
		{
			if (conn == connections[index]) continue;
			Packet packetType = P_ChatMessage;
			if (send(conn, (char*)&packetType, sizeof(Packet), NULL) == SOCKET_ERROR) closesocket(connections[index]);
			if (send(conn, (char*)&msg_size, sizeof(int), NULL) == SOCKET_ERROR) closesocket(connections[index]);
			if (send(conn, msg, msg_size, NULL) == SOCKET_ERROR) closesocket(connections[index]);
		}
		delete[] msg;
	}
	break;
	case P_Close:
	{
		cout << "Client disconnected" << endl;
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
		if (index >= connections.size())
			index--;
		recv(connections[index], (char*)&packetType, sizeof(Packet), NULL);
		if (!ProcessPacket(index, packetType)) break;
	}
	packetType = P_Close;
	send(connections[index], (char*)&packetType, sizeof(Packet), NULL);
	closesocket(connections[index]);
	connections.erase(connections.begin() + index);
	cout << "Connection closed" << endl;
	cout << "Current number of connections " << connections.size() << endl;
	return;
}
