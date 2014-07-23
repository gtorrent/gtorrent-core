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

#include <gtorrent/libtorrent.hpp>
#include <gtorrent/Core.hpp>
#include <gtorrent/Event.hpp>
#include <gtorrent/Log.hpp>
#include <gtorrent/Torrent.hpp>

int main(int argc, char *argv[]) {
  printf("gTest.exe started\n");
  gt::Log::Debug("Testing logging");
    gt::Log::Debug("Starting core...");
    shared_ptr<gt::Core> m_core(new gt::Core);
    gt::Log::Debug("Testing if core is running");
    gt::Log::Debug("%s", m_core->isRunning() ? "Core is running" :
      "Core is not running");
    gt::Log::Debug("Shutting down core...");
    m_core->shutdown();
  return 0;
}
