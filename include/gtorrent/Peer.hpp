#pragma once

#include <libtorrent/peer_info.hpp>

namespace gt
{
	class Peer : public libtorrent::peer_info
	{
		public:
			Peer(const libtorrent::peer_info &p);
			std::string ip;
			std::string uploadSpeed;
			std::string downloadSpeed;
			std::string client;
	};
}
