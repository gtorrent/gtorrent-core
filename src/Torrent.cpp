// Copyright 2014 gtorrent team, GNU GPLv3

#include <string>
#include "Core.hpp"
#include "Torrent.hpp"
#include "Log.hpp"

#define T_PPM 1000000.f

// format 0d 0h 0m 0s
string getTimeString(boost::int64_t t) {
  switch (t) {
      case(t <= 0):
        return string();
        break;
      case(t > 0):
        return << t << "s";
        break;
      case(t >= 60):
        return << (t / 60) - (t % 60) << "m ";
        break;
      case(t >= 3600):
        return << (t / 3600) - (t % 3600) << "h ";
        break;
      case(t >= 86400):
        return << (t / 86400) - (t % 86400) << "d";
        break;
  }
}

string getRateString(boost::int64_t file_rate) {
  ostringstream file_rate_string;
  file_rate_string << getFileSizeString(file_rate) << "/s";
  return file_rate_string.str();
}

string Torrent::getFileSizeString(boost::int64_t fs) {
  std::ostringstream tfs << fixed << setprecision(1);
  switch (fs) {
    case(fs<= 0):
      return string();
      break;
    case(fs > 0 && fs < 1024):
      tfs << fs << " B";
      return tfs;
      break;
    case(fs >= 1024 && fs < 1048576):
      tfs << fs / 1024 << " KB";
      return tfs;
      break;
    case(fs >= 1048576 && fs < 1073741824):
      tfs << fs / 1048576 << " MB";
      return tfs;
      break;
    case(fs >= 1073741824):
      tfs << fs / 1073741824 << " GB";
      return tfs;
      break;
  }
}

gt::Torrent::Torrent(string path) :
  m_path(path) {
  // TODO(general) add argument to allow user to
  // override the default save path of $HOME/Downloads
  setSavePath("");
  if (gt::Core::isMagnetLink(path)) {
    m_torrent_params.url = path;
  } else {
    // libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0
    // and a shared_ptr in svn.
    // Using decltype allows us to make it compatible with both versions
    // while still properly using the constructor to avoid a compiler error
    // on boost 1.55 when the = operator is used with a pointer.
    libtorrent::error_code ec;
    decltype(m_torrent_params.ti) tester = decltype(m_torrent_params.ti)
      (new libtorrent::torrent_info(path, ec));
    if (ec.value() == 0) {
      ifstream torrentcheck(path);
      bool isempty = torrentcheck.peek() == ifstream::traits_type::eof();
      torrentcheck.close();
      if (isempty) {
        gt::Log::Debug("The torrent file was empty");
        throw - 1;
      }
      // If no exception was thrown add the torrent
      m_torrent_params.ti = tester;
    } else {
      // Call deconstructor?
      gt::Log::Debug(ec.message().c_str());
      // Throw error if construction of libtorrent::torrent_info fails.
      throw - 1;
    }
  }
}

void gt::Torrent::setSavePath(string savepath) {
  if (savepath.empty())
    savepath = gt::Core::getDefaultSavePath();
  if (savepath.empty())
    // Fall back to ./ if $HOME is not set
    savepath = "./";
  m_torrent_params.save_path = savepath;
}

bool gt::Torrent::pollEvent(gt::Event &event) {
  if (getTotalProgress() >= 100) {
    event.type = gt::Event::DownloadCompleted;
    return true;
  }
  return false;
}

string gt::Torrent::getTextState() {
  switch (getState()) {
    case libtorrent::torrent_status::checking_files:
     return "Checking";
     break;
    case libtorrent::torrent_status::seeding:
     return "Seeding";
     break;
    case libtorrent::torrent_status::downloading:
    default:
     ostringstream o;
     int precision = 1;
    // m_torrent_params.ti is not initial initialized for magnet links
    if (m_torrent_params.ti != NULL) {
      if (m_torrent_params.ti->total_size() < 1024 * 1024 * 1024) {
        // Set 0 decimal places if file is less than 1 gig.
        precision = 0;
      }
    }
    o << fixed << setprecision(precision) << getTotalProgress() << " %";
    return o.str();
    break;
  }
}

float gt::Torrent::getTotalRatio() {
  if (getTotalDownloaded() > 0) {
    return float(getTotalUploaded()) / float(getTotalDownloaded());
  } else {
    return 0.0f;
  }
}

string gt::Torrent::getTextTotalRatio() {
  ostringstream ttr;
  ttr << fixed << setprecision(3) << getTotalRatio();
  return ttr.str();
}

void gt::Torrent::setPaused(bool isPaused) {
  m_handle.auto_managed(!isPaused);
  if (isPaused)
    m_handle.pause();
  else
    m_handle.resume();
}
