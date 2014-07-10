#ifndef TORRENTENGINE_HPP
#define TORRENTENGINE_HPP

#define BOOST_ASIO_DYN_LINK
#include <boost/filesystem/operations.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/lazy_entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>
#include <memory>

class TorrentEngine;	typedef std::unique_ptr<TorrentEngine> te_ptr;

class TorrentEngine
{
private:
	libtorrent::session m_session;
public:
	TorrentEngine();

	void addTorrent(std::string path);
	void update();
};
#endif