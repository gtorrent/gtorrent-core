//gtorrent - torrent client
//Copyright (C) 2014  gtorrent team  LGPL

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
