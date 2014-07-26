#include "Core.hpp"
#include "Torrent.hpp"
#include "Log.hpp"
#define T_PPM 1000000.f

// format 0d 0h 0m 0s
string getTimeString( boost::int64_t time_s ) {
    if ( time_s <= 0 )
        return "∞";

    boost::int64_t time_m = time_s / 60;
    time_s %= 60;
    boost::int64_t time_h = time_m / 60;
    time_m %= 60;
    boost::int64_t time_d = time_h / 24;
    time_h %= 24;

    ostringstream time_string;

    if ( time_d > 0 )
        time_string << time_d << "d ";
    if ( time_h > 0 )
        time_string << time_h << "h ";
    if ( time_m > 0 )
        time_string << time_m << "m ";
    time_string << time_s << "s";

    return time_string.str();
}

string getRateString(boost::int64_t file_rate) {
    ostringstream frs;
    if (file_rate> 0) {
        frs << getFileSizeString(file_rate) << "/s";
    }
    return frs.str();
}

string getFileSizeString(boost::int64_t file_size) {
    if (file_size <= 0) {
        return string();
    }

    ostringstream fss;
    fss << setprecision(3);

    if (file_size >= (1024 * 1024 * 1024)) {
        fss << file_size / (double)(1024 * 1024 * 1024) << " GB";
    } else if (file_size >= (1024 * 1024)) {
        fss << (file_size / (double)(1024 * 1024)) << " MB";
    } else if (file_size >= 1024) {
        fss << (file_size / (double)1024) << " KB";
    } else if (file_size> 0) {
        fss << file_size<< "B ";
    }
    return fss.str();
}

gt::Torrent::Torrent(string path) : m_path(path) {
    setSavePath(""); //TODO add argument to allow user to override the default save path of $HOME/Downloads
    if (gt::Core::isMagnetLink(path))
        m_torrent_params.url = path;
    else {
        //libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0 and a shared_ptr in svn.
        //Using decltype allows us to make it compatible with both versions while still properly using the constructor to avoid a compiler error on boost 1.55 when the = operator is used with a pointer.
        libtorrent::error_code ec;
        decltype(m_torrent_params.ti) tester = decltype(m_torrent_params.ti)(new libtorrent::torrent_info(path, ec));
        if (ec.value() == 0) {
            ifstream torrentcheck(path);
            bool isempty = torrentcheck.peek() == ifstream::traits_type::eof();
            torrentcheck.close();
            if (isempty) {
                gt::Log::Debug("The torrent file was empty");
                throw - 1;
            }
			cout << "no probs here" << endl;
            m_torrent_params.ti = tester;//If no exception was thrown add the torrent
        } else {
            gt::Log::Debug(ec.message().c_str());//Call deconstructor?
            throw - 1; //Throw error if construction of libtorrent::torrent_info fails.
        }
    }
}

void gt::Torrent::setSavePath(string savepath) {
    if (savepath.empty())
        savepath = gt::Core::getDefaultSavePath();
    if (savepath.empty())
        savepath = "./"; //Fall back to ./ if $HOME is not set
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
    ostringstream o;
    int precision = 1;

    switch (getState()) {
    case libtorrent::torrent_status::queued_for_checking:
        return "Queued for checking";
    case libtorrent::torrent_status::downloading_metadata:
        return "Downloading metadata...";
    case libtorrent::torrent_status::finished:
        return "Finished";
    case libtorrent::torrent_status::allocating:
        return "Allocating...";
    case libtorrent::torrent_status::checking_resume_data:
        return "Resuming...";
    case libtorrent::torrent_status::checking_files:
        return "Checking...";
    case libtorrent::torrent_status::seeding:
        return "Seeding";
    case libtorrent::torrent_status::downloading:
        break;
    }

    if(isPaused())
        return "Paused";

    if (m_torrent_params.ti != NULL) //m_torrent_params.ti is not initial initialized for magnet links
        precision = m_torrent_params.ti->total_size() < 0x2000000;//Set 0 decimal places if file is less than 1 gig.
    o << fixed << setprecision(precision) << getTotalProgress() << '%';
    return o.str();

}

float gt::Torrent::getTotalRatio() {
    if ( getTotalDownloaded() > 0 )
        return float( getTotalUploaded() ) / float( getTotalDownloaded() );
    else
        return 0.0f;
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

vector<bool> gt::Torrent::getPieces() {
    libtorrent::bitfield p = m_handle.status().pieces;
    int n = m_handle.get_torrent_info().num_pieces();
    vector<bool> pieces;
    for(int i = 0; i < n; ++i)
        pieces.push_back(p.get_bit(i));
    return pieces;
}
