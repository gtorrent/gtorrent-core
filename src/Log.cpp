#include "Log.hpp"

namespace gt
{
	string getFullDateNumber(int n)
	{
		ostringstream oss;

		if (n < 10)
			oss << "0" << n;
		else
			oss << n;

		return oss.str();
	}

	string Log::getTimeStamp()
	{
		time_t t = time(0);
		struct tm *lt = localtime(&t);

		// TODO: Maybe convert this stream string structure into something else.

		stringstream oss;

		oss << (1900 + lt->tm_year)	            << "-"
			<< getFullDateNumber(lt->tm_mon)	<< "-"
			<< getFullDateNumber(lt->tm_mday)	<< " @ "
			<< getFullDateNumber(lt->tm_hour)	<< ":"
			<< getFullDateNumber(lt->tm_min)	<< ":"
			<< getFullDateNumber(lt->tm_sec);

		return oss.str();
	}

	void Log::Debug(const char *fmt, ...)
	{
		va_list args;

		printf("[%s]: ", Log::getTimeStamp().c_str());
		vprintf(fmt, args);
		printf("\n");
	}

}
