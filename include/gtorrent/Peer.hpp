#pragma once

#include <libtorrent/peer_info.hpp>

#include "Torrent.hpp"

namespace gt
{
	class Peer : public libtorrent::peer_info
	{
		public:
			Peer(const libtorrent::peer_info &p);
			inline boost::asio::ip::address getIP() const
			{
				return ip.address();
			}
			inline int64_t getUploadSpeed() const
			{
				return up_speed;
			}
			inline int64_t getDownloadSpeed() const
			{
				return down_speed;
			}
			std::string getClient() const;
	};
}
