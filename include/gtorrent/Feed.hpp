#pragma once

#include <string>
#include <memory>
#include <vector>
#include <libtorrent/rss.hpp>
#include <Torrent.hpp>
#include <Core.hpp>

namespace gt
{
	class Feed : public libtorrent::feed_handle
	{
		gt::Core *core;
	public:
		typedef std::function<void(std::shared_ptr<gt::Feed>)> feedCallback;
		typedef std::function<void(libtorrent::feed_item, const Feed &)> itemCallback;

		std::function<void(int, std::shared_ptr<gt::Feed>)> onStateChanged;

		// Ordered in the important to less important callbacks
		feedCallback onUpdateFinished;
		feedCallback onUpdateErrored;
		itemCallback onNewItemAvailable;
		feedCallback onUpdateStarted;

		Feed(std::string Url, gt::Core *Core, feedCallback finishedCallback = feedCallback(), feedCallback erroredCallback = feedCallback(), itemCallback onNewItemAvailable = itemCallback(), feedCallback startedCallback = feedCallback());

		std::vector<libtorrent::feed_item> getFilteredItems(std::function<bool(std::string)> filterFun);
		std::vector<gt::Torrent> addFilteredItems(std::function<bool(std::string)> filterFun);
		bool operator==(const libtorrent::feed_handle& rhs) { return rhs.settings().url == settings().url; }
	};
}
