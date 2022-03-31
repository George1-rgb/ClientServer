#include "Logger.h"
#include <chrono>

Logger::Logger()
{
	out_file.open("Log.txt", ios_base::app);
	if (!out_file.is_open())
	{
		cout << "Can't opne log file!" << endl;
		return;
	}
}

void Logger::log(LogType type, string callingSection, string message)
{
	switch (type)
	{
	case Message: out_file << "|MESSAGE| from " << "|" << callingSection << "| "; break;
	case Warning: out_file << "|WARNING| from " << "|" << callingSection << "| "; break;
	case Error: out_file << "|ERROR| from " << "|" << callingSection << "| "; break;
	default: cout << "Unrecognized LogType" << endl; break;
	}
	out_file << message << "\n";
}

Logger::~Logger()
{
	out_file.close();
}
