#pragma once

#include <string>

namespace gt
{
	class Log
	{
	public:
		static std::string getTimeStamp();
		static void Debug(const char *fmt, ...);
	};
};

