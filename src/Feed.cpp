#include <Feed.hpp>
#include <Log.hpp>
#include <regex>

using namespace std;

// load default feed settings but change them quickly later
gt::Feed::Feed(libtorrent::feed_handle fe) :
	feed_handle(fe)
{
	update_feed();
}
