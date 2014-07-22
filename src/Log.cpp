// Copyright 2014 gtorrent team, GNU GPLv3

#include <stdarg.h>
#include <string>
#include <sstream>
#include <cstdio>
#include <ctime>
#include "Log.hpp"

string getFullDateNumber(int n) {
  ostringstream oss;

  if (n < 10) {
    oss << "0" << n;
  }  else {
    oss << n;
  }

  return oss.str();
}

string gt::Log::getTimeStamp() {
  time_t t = time(0);
  struct tm *lt = localtime_r(&t);

  // TODO(general): Maybe convert this stream string structure
  // into something else.

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

void gt::Log::Debug(const char *fmt, ...) {
  FILE * pFile = fopen("gtorrent.log", "a");
  va_list args;
  va_start(args, fmt);
  printf("[%s]: ", gt::Log::getTimeStamp().c_str());
  fprintf(pFile, "[%s]: ", gt::Log::getTimeStamp().c_str());
  vprintf(fmt, args);
  vfprintf(pFile, fmt, args);
  printf("\n");
  fprintf(pFile, "\n");
  va_end(args);
}
