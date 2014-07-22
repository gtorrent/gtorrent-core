// Copyright 2014 gtorrent team, GNU GPLv3

#pragma once

#include <string>

namespace gt {
  class Log {
   public:
     static string getTimeStamp();
     static void Debug(const char *fmt, ...);
  };
}
