#pragma once

#include <string>
#include <sstream>
#include <ctime>

using namespace std;

namespace gt
{
	class Log
	{
	public:
		static string getTimeStamp();
		static void Debug(const char *fmt, ...);
	};
}
