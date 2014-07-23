
/* gtorrent - torrent client
    Copyright (C) 2014  gtorrent team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <string>
#include <ctime>
#include <sstream>
#include "Log.hpp"

string getFullDateNumber(int n) {
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
  va_list args;

  printf("[%s]: ", gt::Log::getTimeStamp().c_str());
  vprintf(fmt, args);
  printf("\n");

}
