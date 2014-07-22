#pragma once

namespace gt
{
	class Log
	{
	public:
		static string getTimeStamp();
		static void Debug(const char *fmt, ...);
	};
}
