#include <iostream>
#include "Client.h"
using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	Client client;
	client.Connect("192.168.0.57", 1111);
	
	system("pause");

	return 0;
}