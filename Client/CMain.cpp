#include <iostream>
#include "Client.h"
#include <conio.h>
using namespace std;

int main(int argc, char* argv[])
{

	HWND hwnd = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(hwnd, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);

	setlocale(LC_ALL, "Russian");
	Client client;
	client.Connect("192.168.0.57", 1111);
	
	//system("pause");

	return 0;
}