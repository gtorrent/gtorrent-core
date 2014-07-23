
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
#include "Core.hpp"
#include "Torrent.hpp"
#include "Log.hpp"
#define T_PPM 1000000.f

// format 0d 0h 0m 0s
string getTimeString(boost::int64_t s) {
  if ( s <= 0 )
    return "???";

  boost::int64_t m = s / 60;
  s %= 60;
  boost::int64_t h = m / 60;
  m %= 60;
  boost::int64_t d = h / 24;
  h %= 24;

  ostringstream string;

  if ( d > 0 )
    string << d << "d ";
  if ( h > 0 )
    string << h << "h ";
  if ( m > 0 )
    string << m << "m ";
  string << s << "s";

  return string.str();
}

string getRateString(boost::int64_t fr) {
  ostringstream frs;
  frs << getFileSizeString(fr) << "/s";
  return frs.str();
}

string getFileSizeString(boost::int64_t fs) {
  if (fs <= 0) {
    return string();
  }
  ostringstream fss;
  fss << fixed << setprecision(3);
  if (fs >= (1073741824)) {
    fss <<  fixed << setprecision(3)<< fs / 1073741824 << " GB";
  }
  if (fs >= (1048576) && fs < (1073741824)) {
    fss <<  fixed << setprecision(3) << (fs / 1048576) << " MB";
  }
  if (fs >= 1024 && fs < (1048576)) {
    fss << fixed << setprecision(3) << (fs / 1024) << " KB";
  }
  if (fs > 0 && fs < 1024) {
    fss << fs << " B";
  }
  return fss.str();
}

gt::Torrent::Torrent(string path) :
  m_path(path) {
  /// TODO add argument to allow user to override the default save path
  // of $HOME/Downloads
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
  if (savepath.empty()) {
    savepath = gt::Core::getDefaultSavePath();
  }
  if (savepath.empty()) {
      // Fall back to ./ if $HOME is not set
      savepath = "./";
  }
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
  if ( getTotalDownloaded() > 0 ) {
    return float( getTotalUploaded() ) / float( getTotalDownloaded() );
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
  if ( isPaused )
    m_handle.pause();
  else
    m_handle.resume();
}
