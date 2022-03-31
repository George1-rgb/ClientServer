#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
enum LogType
{
	Message = 0,
	Warning,
	Error
};

class Logger
{
public:
	Logger();
	void log(LogType type, string callingSection, string message);
	~Logger();
private:
	ofstream out_file;
};

