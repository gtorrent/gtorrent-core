#pragma once

#include <string>
#include <memory>
#include <Core.hpp>
#include <Torrent.hpp>
#include <libtorrent/rss.hpp>


namespace gt
{
	// Is used to manage a feed group and serialization/deserialization
	class FeedGroup
	{
	public:
		std::string name;
		gt::Core *core;
		std::vector<std::shared_ptr<gt::Feed>> m_feeds;
		int updating;
		std::set<std::string> functions;
		std::map<std::string, std::string> filters;
		void addFunction(std::string);
		void addItem(const libtorrent::feed_item &fi);
		operator std::string();
		std::set<std::string> &getFunctions();
		bool contains(libtorrent::feed_handle fh);
		std::vector<std::shared_ptr<gt::Torrent>> addFilteredItems(std::function<bool(std::string)> filterFun);
		std::vector<std::shared_ptr<gt::Torrent>> addFilteredItems();

		std::vector<libtorrent::feed_item> getFilteredItems(std::function<bool(std::string)> filterFun);
		std::vector<libtorrent::feed_item> getFilteredItems();
		static std::vector<std::shared_ptr<gt::FeedGroup>> fromString(std::string sData);

		typedef std::function<void(std::shared_ptr<gt::Feed>)> feedCallback;
		typedef std::function<void(const libtorrent::feed_item&, std::shared_ptr<gt::Feed>)> itemCallback;
		typedef std::function<void(int, std::shared_ptr<gt::Feed>)> stateCallback;

		stateCallback onStateChanged    ;
		feedCallback  onUpdateFinished  ;
		feedCallback  onUpdateErrored   ;
		itemCallback  onNewItemAvailable;
		feedCallback  onUpdateStarted   ;

	};
}
