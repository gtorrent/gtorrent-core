#include "Peer.hpp"

#include "Torrent.hpp"


gt::Peer::Peer(const libtorrent::peer_info &p) :
	ip(p.ip.address().to_string()),
	uploadSpeed(getRateString(p.up_speed)),
	downloadSpeed(getRateString(p.down_speed)),
	client(p.client)
{}
