#include "Peer.hpp"

#include "Torrent.hpp"


gt::Peer::Peer(const libtorrent::peer_info &p) :
	peer_info(p)
{
}

std::string gt::Peer::getClient() const
{
	std::string str = pid.to_string();
	if(str[0] == 'G' && str[1] == 'T')
		return "gTorrent"; // Use this until our client is in the supported list of libtorrent
	else
		return client;
}
