#include "Log.hpp"
#include <sstream>
#include <ctime>
#include <stdarg.h>

std::string getFullDateNumber(int n)
{
	if (n < 10)
		return std::string("0") + std::to_string(n);
	else
		return std::to_string(n);
}

std::string gt::Log::getTimeStamp()
{
	time_t t = time(0);
	struct tm *lt = localtime(&t);

	return std::to_string(1900 + lt->tm_year) + "-"   +
	       getFullDateNumber(lt->tm_mon)    + "-"   +
	       getFullDateNumber(lt->tm_mday)   + " @ " +
	       getFullDateNumber(lt->tm_hour)   + ":"   +
	       getFullDateNumber(lt->tm_min)    + ":"   +
	       getFullDateNumber(lt->tm_sec);

}

int gt::DebugLevel = 0;

// 0 = None, 1 = Warning, 2 = Critical, 3 = Severe, 4 = All
void gt::Log::Debug(std::string debug, int level)
{
#if USE_LOGGING
	if(level < DebugLevel) return;
	FILE *pFile = fopen("gtorrent.log", "a");

	if (!pFile)
		perror("fopen()");
	else
	{
		fprintf(pFile, "[%s]: %s\n", gt::Log::getTimeStamp().c_str(), debug.c_str());
	}
	printf("[%s]: %s\n", gt::Log::getTimeStamp().c_str(), debug.c_str());

	if (pFile)
		fclose(pFile);
#endif
}
