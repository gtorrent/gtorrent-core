#pragma once

#include "all.h"

namespace gt
{
	class Log
	{
	public:
		static string getTimeStamp();
		static void Debug(const char *fmt, ...);
	};
}
