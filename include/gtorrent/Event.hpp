// Copyright 2014 gtorrent team, GNU GPLv3

#pragma once

namespace gt {
  struct Event {
    enum Type {
      DownloadCompleted
    };

    Type type;
  };
}
