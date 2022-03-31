#pragma once
#include "Logger.h"
class Global
{
public:
	static Global& instance();
	Logger& getLogger() { return *log; }
private:
	Global();
	~Global();
	static Global* Instance;
	Logger* log;
};

