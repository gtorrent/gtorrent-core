#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>
#include <libtorrent/rss.hpp>
#include <Torrent.hpp>
#include <Core.hpp>
#include "FeedGroup.hpp"


namespace gt
{
	class Feed : public libtorrent::feed_handle
	{

	public:
		std::vector<std::shared_ptr<gt::FeedGroup>> owners;

		// When a feed changes state, all of its groups are informed
		Feed(const libtorrent::feed_handle &fe);

		bool operator==(const libtorrent::feed_handle& rhs) { return rhs.settings().url == settings().url; }
	};
}
