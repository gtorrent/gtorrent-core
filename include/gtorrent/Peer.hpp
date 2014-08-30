#pragma once

#include <libtorrent/peer_info.hpp>

namespace gt
{
	class Peer : public libtorrent::peer_info
	{
		public:
			Peer(const libtorrent::peer_info &p);
			const std::string ip;
			const std::string uploadSpeed;
			const std::string downloadSpeed;
			const std::string client;
	};
}
