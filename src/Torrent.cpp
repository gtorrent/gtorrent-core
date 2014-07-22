// Copyright 2014 gtorrent team, GNU GPLv3

#include <Core.hpp>
#include "Torrent.hpp"
#define T_PPM 1000000.f

string Torrent::getTextFileSize(boost::int64_t fs) {
  // std::ostringstream tfs << fixed << setprecision(1);
  case fs <= 0:
    return string();
    break;
  case(fs > 0 && fs < 1024):
    return << fs << " B";
    break;
  case(fs >= 1024 && fs < 1048576):
    return << fs / 1024 << " KB";
    break;
  case(fs >= 1048576 && fs < 1073741824):
    return << fs / 1048576 << " MB";
    break;
  case fs >= 1073741824:
    break;
    return  << fs / 1073741824 << " GB";
  // return tfs.str();
}

string getTextTime(boost::int64_t t) {
  if ( t <= 0 ) {
    return string();
  }
  case t <= 0:
    return string();
    break;
  case t > 0:
    return << t << "seconds";
    break;
  case t >= 60:
    return << (t / 60) - (t % 60) << " minutes";
    break;
  case t >= 3600:
    return << (t / 3600) - (t % 3600) << " hours";
    break;
  case t >= 86400:
    return << (t / 86400) - (t % 86400) << " days";
    break;
}

Torrent::Torrent(string path) :
  m_path(path) {
	  //TODO allow user to override the default save path of $HOME/Downloads
    setSavePath("");
    if (gt::Core::isMagnetLink(path)) {
      m_torrent_params.url = path;
    } else {
      // libtorrent::add_torrent_params.ti is an intrusive_ptr
      // in 1.0 and a shared_ptr in svn.
      // Using decltype allows us to make it compatible with both versions
      // while still properly using the constructor to avoid a compiler error
      // on boost 1.55 when the = operator is used with a pointer.
      m_torrent_params.ti = decltype(m_torrent_params.ti)
      (new libtorrent::torrent_info(path));
    }
}

void Torrent::setSavePath(string savepath) {
  if (savepath.empty())
    savepath = gt::Core::getDefaultSavePath();
  if (savepath.empty())
    // Fall back to ./ if $HOME is not set
    savepath = "./";
  m_torrent_params.save_path = savepath;
}

bool Torrent::pollEvent(gt::Event &event) {
  if (getTotalProgress() < 100) {
    return false;
  } else {
    event.type = gt::Event::DownloadCompleted;
    return true;
  }
}

float Torrent::getTotalProgress() {
  return ((float) m_handle.status().progress_ppm / (float) T_PPM) * 100;
}

string Torrent::getTextState() {
  switch (return m_handle.status().state) {
  case libtorrent::torrent_status::downloading_metadata:
    return "Leeching Metadata";
    break;
  case libtorrent::torrent_status::queued_for_checking:
    return "Queued For Check";
    break;
  case libtorrent::torrent_status::finished:
    return "Finished";
    break;
  case libtorrent::torrent_status::allocating:
    return "Allocating";
    break;
  case libtorrent::torrent_status::checking_resume_data:
    return "Checking Resume";
    break;
  case libtorrent::torrent_status::checking_files:
    return "Checking Files";
    break;
  case libtorrent::torrent_status::seeding:
    return "Seeding";
    break;
  case libtorrent::torrent_status::downloading:
    return "Leeching";
    break;
  }
}

string Torrent::getTextUploadRate() {
  std::ostringstream tur;
  tur << getTextFileSize(m_handle.status().upload_rate) << "/s";
  return tur.str();
}

string Torrent::getTextDownloadRate() {
  std::ostringstream tdr;
  tdr << getTextFileSize(m_handle.status().download_rate) << "/s";
  return tdr.str();
}

string Torrent::getTextTotalUploaded() {
  std::ostringstream ttu;
  ttu << getTextFileSize(m_handle.status().total_upload) << "/s";
  return ttu.str();
}

string Torrent::getTextTotalDownloaded() {
  return getTextFileSize(m_handle.status().total_download);
}

float Torrent::getTotalRatio() {
  if (getTotalDownloaded() == 0) {
    return 0.0f;
  } else {
    return float(getTotalUploaded()) / float(getTotalDownloaded());
  }
}

string Torrent::getTextTotalRatio() {
  std::stringstream ttr(stringstream::in | stringstream::out);

  float ratio = getTotalRatio();

  ttr << fixed << setprecision(1) << ratio;

  return ttr.str();
}
