#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>
#include <libtorrent/rss.hpp>
#include <libtorrent/alert_types.hpp>
#include <Torrent.hpp>
#include <Core.hpp>
#include <alert_types.hpp>
#include "FeedGroup.hpp"

namespace gt
{
	class Feed : public libtorrent::feed_handle
	{

	public:
		std::set<std::shared_ptr<gt::FeedGroup>> owners;

		// Question: Does this really need a reference to itself? See how it's being used so far and determine if the args can be simplified.
		// Should move this to a pure virtual class/interface
		std::function<void(int, gt::Feed*)> onStateChanged; // why does this remind me of kirby <('_')>

		std::function<void(libtorrent::rss_item_alert*)> onItemAlert;

		// When a feed changes state, all of its groups are informed
		Feed(libtorrent::feed_handle fe);

		bool operator==(const libtorrent::feed_handle& rhs) { return rhs.get_feed_status().url == get_feed_status().url; }
	};
}
