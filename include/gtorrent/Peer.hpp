#pragma once

#include <libtorrent/peer_info.hpp>

#include "Torrent.hpp"

namespace gt
{
	class Peer : public libtorrent::peer_info
	{
		public:
			Peer(const libtorrent::peer_info &p);
			inline std::string getIP() const
			{
				return ip.address().to_string();
			}
			inline std::string getUploadSpeed() const
			{
				return std::to_string(up_speed);
			}
			inline std::string getDownloadSpeed() const
			{
				return std::to_string(down_speed);
			}
			std::string getClient() const;
	};
}
