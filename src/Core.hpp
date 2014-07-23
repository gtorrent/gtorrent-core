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

#pragma once

using namespace std;

#include <vector>
#include <memory>
#include <string>
#include "Torrent.hpp"

namespace gt {
  class Core {
   private:
    vector<shared_ptr<Torrent> > m_torrents;
    libtorrent::session m_session;
    bool m_running;
   public:
    Core();

    static bool isMagnetLink(string const& link);

    static string getDefaultSavePath();
    inline vector< shared_ptr< Torrent > > &getTorrents() {
      return m_torrents;
    }
    shared_ptr<Torrent> addTorrent(string path);

    inline bool isRunning() const {
      return m_running;
    }
    void shutdown();
    void update();
  };
}
