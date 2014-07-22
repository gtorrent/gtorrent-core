// Copyright 2014 gtorrent team, GNU GPLv3

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "libtorrent.hpp"
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
      vector<shared_ptr<Torrent> > &getTorrents();
      shared_ptr<Torrent> addTorrent(string path);

      bool isRunning();
      void shutdown();
      void update();
  };
}
