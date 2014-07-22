// Copyright 2014 gtorrent team, GNU GPLv3

#pragma once

#include <iomanip>
#include <sstream>
#include <string>
#include "libtorrent.hpp"
#include "Event.hpp"


class Torrent {
 private:
  unsigned int m_id;
  libtorrent::add_torrent_params m_torrent_params;
  libtorrent::torrent_handle m_handle;
  string m_path;

 public:
  Torrent(string path);

  bool pollEvent(gt::Event &event);

  // Returns the current download rate of the torrent
  unsigned int getDownloadRate();

  // Returns the current amount of data uploaded for this torrent
  boost::int64_t getTotalUploaded();

  // Returns the current amount of data downloaded for this torrent
  boost::int64_t getTotalDownloaded();

  // Returns the ratio (uploaded/downloaded) for this torrent
  float getTotalRatio();

  // Returns a friendly string for the torrent state
  string getTextState();

  // Returns a friendly string for the current upload rate
  string getTextUploadRate();

  // Returns a friendly string for the current download rate
  string getTextDownloadRate();

  // Returns a friendly string for the current upload total
  string getTextTotalUploaded();

  // Returns a friendly string for the current download total
  string getTextTotalDownloaded();

  // Returns a friendly string for the current ratio
  string getTextTotalRatio();

  // Setters
  void setHandle(libtorrent::torrent_handle &h);
  void setSavePath(string savepath);
};
