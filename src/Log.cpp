#include "Log.hpp"
#include <sstream>
#include <ctime>
#include <stdarg.h>

using namespace std;

string getFullDateNumber(int n)
{
	ostringstream oss;

	if (n < 10)
	{
		oss << "0" << n;
	}
	else
	{
		oss << n;
	}

	return oss.str();
}

string gt::Log::getTimeStamp()
{
	time_t t = time(0);
	struct tm *lt = localtime(&t);

	// TODO: Maybe convert this stream string structure into something else.

	stringstream oss;

	oss << (1900 + lt->tm_year);
	oss << "-";
	oss << getFullDateNumber(lt->tm_mon);
	oss << "-";
	oss << getFullDateNumber(lt->tm_mday);
	oss << " @ ";
	oss << getFullDateNumber(lt->tm_hour);
	oss << ":";
	oss << getFullDateNumber(lt->tm_min);
	oss << ":";
	oss << getFullDateNumber(lt->tm_sec);

	return oss.str();
}

void gt::Log::Debug(const char *fmt, ...)
{
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
}
