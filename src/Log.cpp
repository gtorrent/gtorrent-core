#include "Log.hpp"
#include <sstream>
#include <ctime>
#include <stdarg.h>

static string gt::Log::getFullDateNumber(int n) {
    ostringstream oss;

    if (n < 10) {
        oss << "0" << n;
    } else {
        oss << n;
    }

    return oss.str();
}

string gt::Log::getTimeStamp() {
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

gt::Log::Debug::Debug() : m_logFile("gtorrent.log", fstream::out | fstream::app) {
	*this << "[" << gt::Log::getTimeStamp() << "]: ";
}

gt::Log::Debug::~Debug() {
	
	*this << "\n";
	
	m_logFile.close();
}