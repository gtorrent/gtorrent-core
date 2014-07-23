
//gtorrent - torrent client
//Copyright (C) 2014  gtorrent team  LGPL

#pragma once

namespace gt {
  struct Event {
    enum Type {
        DownloadCompleted
    };

    Type type;
  };
}
