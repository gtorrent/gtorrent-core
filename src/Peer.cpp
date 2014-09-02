#include "Peer.hpp"

#include "Torrent.hpp"


gt::Peer::Peer(const libtorrent::peer_info &p) :
	ip(p.ip.address().to_string()),
	uploadSpeed(getRateString(p.up_speed)),
	downloadSpeed(getRateString(p.down_speed)),
	client(p.client)
{
	std::string str = p.pid.to_string();
	if(str[0] == 'G' && str[1] == 'T') client = "gTorrent"; // Use this until our client is in the supported list of libtorrent
}
