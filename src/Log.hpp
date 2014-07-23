
//gtorrent - torrent client
//Copyright (C) 2014  gtorrent team  LGPL

#pragma once

using namespace std;

#include <string>

namespace gt {
  class Log {
   public:
    static string getTimeStamp();
    static void Debug(const char *fmt, ...);
  };
}
