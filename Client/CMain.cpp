#include <iostream>

#include "Client.h"
#include <conio.h>
using namespace std;

int main(int argc, char* argv[])
{

	HWND hwnd = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(hwnd, FALSE);
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Client client;
	client.Connect("192.168.0.57", 1111);

	return 0;
}