#include "Client.h"
#include <iostream>
#include "Types.h"
#include <mutex>
using namespace std;
mutex mtx;
Client::Client()
{
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != NULL) 
	{
		cout << "ОШИБКА #" << WSAGetLastError() << endl;
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
		cout << "ОШИБКА # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
	}
	else
	{
		cout << "***Подлючение к серверу выполнено!***"
			<< "\n!!!ВНИМАНИЕ! Перед закрытием приложения нужно обязательно отключиться от сервера!!!\n***Для отключения введите 'XXX'***" << endl;
		if (!Authorization())
		{
			cout << "Ошибка авторицации!" << endl;
			return;
		}
		cout << "Добро пожаловать на сервер " << userName << "!" << endl;
		thread th([this]()
			{
				ClientHandler();
			});
		th.detach();
		SendingMess();
	}
}

bool Client::Authorization()
{
	bool isAuthorize = false;
	cout << "Введите свое имя: ";
	getline(cin, userName);
	cout << endl;
	Packet packetType = P_Authorize;
	send(ClientSock, (char*)&packetType, sizeof(Packet), NULL);
	int msg_size = userName.size();
	send(ClientSock, (char*)&msg_size, sizeof(int), NULL);
	send(ClientSock, userName.c_str(), msg_size, NULL);
	recv(ClientSock, (char*)&isAuthorize, sizeof(bool), NULL);
	return isAuthorize;
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
	string cin_str;
	string msg;												// The size of sending / receiving packet in bytes
	while (true)
	{
		mtx.lock();
		getline(cin, cin_str);
		if (cin_str == "XXX" || cin_str == "ХХХ")
		{
			Close();
			break;
		}
		else
		{ 
			msg = userName + ": " + cin_str;
			Packet packetType = P_ChatMessage;
			send(ClientSock, (char*)&packetType, sizeof(Packet), NULL);
			int msg_size = msg.size();
			send(ClientSock, (char*)&msg_size, sizeof(int), NULL);
			send(ClientSock, msg.c_str(), msg_size, NULL);
			Sleep(10);
		}
		mtx.unlock();
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
			cout << "Невозможно получить сообщение от сервера" << endl;
			closesocket(ClientSock);
			WSACleanup();
		}
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		packet_size = recv(ClientSock, msg, msg_size, NULL);
		if (packet_size == SOCKET_ERROR)
		{
			cout << "Невозможно получить сообщение от сервера" << endl;
			closesocket(ClientSock);
			WSACleanup();
		}
		cout << "\n------------------------------------------------------------------------------" << endl;
		cout << msg << endl;
		delete[] msg; 
	}
	break;
	case P_Close:
	{
		closesocket(ClientSock);
		cout << "***ОТКЛЮЧЕНО***\n***Теперь вы можете закрыть окно***" << endl;
		return false;
	}
	break;
	default:
	{
		cout << "Неизвестный тип пакета: " << packetType << endl;
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

