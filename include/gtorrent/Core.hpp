#pragma once

using namespace std;

#include <vector>
#include <memory>
#include <string>
#include "libtorrent.hpp"
#include "Torrent.hpp"

namespace gt
{
	class Core
	{
	private:
		vector<shared_ptr<gt::Torrent> > m_torrents;
		libtorrent::session m_session;
	public:
		Core();

		static bool isMagnetLink(string const& link);

		static string getDefaultSavePath();

		vector<shared_ptr<gt::Torrent> > &getTorrents();
		shared_ptr<gt::Torrent> addTorrent(string path);
	};
}
