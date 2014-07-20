#pragma once

#include <string>

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
