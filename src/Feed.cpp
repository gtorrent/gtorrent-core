#include <Feed.hpp>
#include <Log.hpp>

using namespace std;

// load default feed settings but change them quickly later
gt::Feed::Feed(string Url, gt::Core *Core, feedCallback startedCallback, feedCallback finishedCallback, feedCallback erroredCallback) : 
	feed_handle(Core->m_session.add_feed(libtorrent::feed_settings())),
	core(Core),
	onUpdateStarted(startedCallback),
	onUpdateFinished(finishedCallback),
	onUpdateErrored(erroredCallback)
{
	using namespace libtorrent;
	feed_settings fs;
	fs.url = Url;
	fs.auto_download = false;
	fs.auto_map_handles = true;
	fs.default_ttl = 5;
	set_settings(fs);
	update_feed();
}

/*void addItem(libtorrent::feed_item &fi)
{
	
}
*/

vector<libtorrent::feed_item> gt::Feed::getFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<libtorrent::feed_item> ret;
	auto fs = get_feed_status();
	for(auto item : fs.items)
		if(filterFun(item.title))
			ret.push_back(item);
	return ret;
}

vector<gt::Torrent> gt::Feed::addFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<gt::Torrent> ret;
	for(auto item : getFilteredItems(filterFun))
		core->addTorrent(item.url);
	return ret;	
}
