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

void gt::Log::Debug(const char *fmt, ...)
{
#if USE_LOGGING
	FILE * pFile = fopen("gtorrent.log", "a");

	va_list args, fileargs;
	va_start(args, fmt);
	va_start(fileargs, fmt); // can't use a varargs twice

	if (!pFile)
	{
		perror("fopen()");
	}
	else
	{
		fprintf(pFile, "[%s]: ", gt::Log::getTimeStamp().c_str());
		vfprintf(pFile, fmt, fileargs);
		fprintf(pFile, "\n");
	}

	printf("[%s]: ", gt::Log::getTimeStamp().c_str());
	vprintf(fmt, args);
	printf("\n");

	va_end(args);
	va_end(fileargs);
	fclose(pFile);
#else
        return;
#endif
}
