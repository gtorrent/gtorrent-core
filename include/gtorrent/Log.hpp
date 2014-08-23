#pragma once

#include <string>

namespace gt
{
	extern int DebugLevel;
	class Log
	{
	public:
		static std::string getTimeStamp();
		static void Debug(std::string debug, int level = 4);
	};
}
