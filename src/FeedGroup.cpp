#include <FeedGroup.hpp>
#include <Log.hpp>
#include <Feed.hpp>
#include <regex>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;

void gt::FeedGroup::addItem(const libtorrent::feed_item &fi)
{
	gt::Log::Debug("Adding " + fi.title + "\t :^)");
	std::string str(fi.url); // We remove some html escape codes
	unsigned i;
	if((i = str.find("amp;")) != str.size()) str.erase(i, 4);
	core->m_pendingTorrents.push_front(core->addTorrent(str));
}

vector<libtorrent::feed_item> gt::FeedGroup::getFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<libtorrent::feed_item> ret;
	for(auto f : m_feeds)
		for(auto item : f->get_feed_status().items)
			if(filterFun(item.title))
				ret.push_back(item);
	return ret;
}

vector<libtorrent::feed_item> gt::FeedGroup::getFilteredItems()
{
	std::function<bool(string&, string&)> bcomp = [](string &a, string &b)
		{
			if(a.empty() || b.empty()) return false;
			if(std::all_of(b.begin(), b.end(), [](char c){ return c >= '0' && c <= '9'; })) // if b is only digits
			{
				if(!std::all_of(a.begin(), a.end(), [](char c){ return c >= '0' && c <= '9'; }))
					return false; //a is not a number
				return stoi(a) < stoi(b);
			}// I don't know why would people compare rel between string for a filter, but there ya go
			return a < b;
		};

	std::function<bool(string, string)> comp[]
	{
		[bcomp](string a, string b) { return  bcomp(a, b); }, // <
		[bcomp](string a, string b) { return  bcomp(b, a); }, // >
		[bcomp](string a, string b) { return !bcomp(b, a) && !bcomp(a, b); }, // =
		[bcomp](string a, string b) { return !bcomp(b, a) || !bcomp(a, b); }  // !
	};

	vector<libtorrent::feed_item> ret;
	if(filters.empty()) 
		for(auto f : m_feeds)
			for(auto item : f->get_feed_status().items)
				ret.push_back(item);

	for(auto f : m_feeds)
	{
		auto fs = f->get_feed_status();
		for(auto function : functions)
		{
			string comparisonOps = "<>=! ";
			string columnName = string(function.begin(),
									   std::find_first_of(function.begin(), function.end(), comparisonOps.begin(), comparisonOps.end())); // yup
			std::regex reg(filters[columnName]);

			std::smatch s;
			char op = *std::find_first_of(function.begin(), function.end(), comparisonOps.begin(), comparisonOps.end() - 1);
			unsigned opIndex = comparisonOps.find(op);
			if(opIndex == std::string::npos) continue; // function is fucked

			std::function<bool(string, string)> compFun = comp[opIndex];
			string literal = function.substr(function.find_last_of(comparisonOps) + 1);

			for(auto item : fs.items)
			{
				std::regex_search(item.title, s, reg); // only one group should be matched here;

				for(auto m : s)
					if(compFun(m, literal))
						ret.push_back(item);
			}
		}
	}
	return ret;
}

vector<shared_ptr<gt::Torrent>> gt::FeedGroup::addFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<shared_ptr<gt::Torrent>> ret;
	for(auto item : getFilteredItems(filterFun))
		ret.push_back(core->addTorrent(item.url));
	return ret;
}

vector<shared_ptr<gt::Torrent>> gt::FeedGroup::addFilteredItems()
{
	vector<shared_ptr<gt::Torrent>> ret;
	for(auto item : getFilteredItems())
		ret.push_back(core->addTorrent(item.url));
	return ret;	
}

void gt::FeedGroup::addFunction(std::string function)
{
	if(functions.find(function) == functions.end())
		return;
	functions.insert(function);
}

std::set<std::string> &gt::FeedGroup::getFunctions()
{
	return functions;
}


// TODO: For this to work as shown below, make the tokenizer skip leading/trailing whitespace around tokens []{},|
/*
 *  //compact
 *  [group name]
 *  { feedurl1, feedurl2 }
 *  { filter1|regex1, filter2|regex2 }
 *  { function1, function2 }
 *
 *
 *  [group name]
 *  {
 *      feedurl1,
 *      feedurl2
 *	}
 *  {
 *      filter1|regex1,
 *      filter2|regex2
 *  }
 *  {
 *      function1,
 *      function2
 *  }
 *
 *
 */

std::vector<std::shared_ptr<gt::FeedGroup>> gt::FeedGroup::fromString(std::string sData, gt::Core *m_core)
{
	using namespace boost;
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

	std::vector<std::shared_ptr<gt::FeedGroup>> ret;
	char_separator<char> sep("", "[]{},|");
	tokenizer tokens(sData, sep);
	for(auto tokenIt = tokens.begin(); tokenIt != tokens.end() && *tokenIt == "[";)
	{
		auto feedg = make_shared<gt::FeedGroup>();

		// group name
		feedg->name = *(++tokenIt);
		trim(feedg->name);

		// Feed URLs
		while(*tokenIt != "{" && ++tokenIt != tokens.end());
		do
		{
			++tokenIt;
			string str = *tokenIt;
			trim(str);
			feedg->m_feeds.push_back(m_core->addFeed(str));
		} while (*(++tokenIt) == ",");

		// filters
		while(*tokenIt != "{" && ++tokenIt != tokens.end());
		do
		{
			string fname, regex;
			++tokenIt;
			fname = *tokenIt++;
			trim(fname);
			while(*tokenIt != "," || *tokenIt != "}")
			{
				regex += *tokenIt++;
				trim(regex);
				if(*tokenIt == "," || *tokenIt == "}") break;
			}
			feedg->filters[fname] = regex;
		} while (*tokenIt == ",");

		//fun
		while(*tokenIt != "{" && ++tokenIt != tokens.end());
		do
		{
			string function;
			++tokenIt;
			function = *tokenIt++;
			trim(function);
			feedg->functions.insert(function);
		} while (*tokenIt == ",");
		// we're done with this feed 
		ret.push_back(feedg);
		while(tokenIt != tokens.end() && *tokenIt != "[") ++tokenIt; // skip shitty whitespace
		
	}
	return ret;
}

bool gt::FeedGroup::contains(libtorrent::feed_handle fh)
{
	for(auto f : m_feeds)
		if(*f == fh)
			return true;
	return false;
}

gt::FeedGroup::operator string()
{
	// TODO: check if it works
	std::string str;
	// group name
	str += "[" + name + "]\n";
	auto i = m_feeds.begin();

	// feed urls
	str += "{\n\t";
	while(1)
	{
		str += (*i)->settings().url;
		if(++i == m_feeds.end()) break;
		str += ",\n\t";
	}
	str += "\n}\n";

	//filters
	auto pair = filters.begin();
	str += "{\n\t";
	while(1)
	{
		str += pair->first + "|" + pair->second;
		if(++pair == filters.end()) break;
		str += ",\n\t";
	}
	str += "\n}\n";

	//functions
	auto fun = functions.begin();
	str += "{\n\t";
	while(1)
	{
		str += *fun;
		if(++fun == functions.end()) break;
		str += ",\n\t";
	}
	str += "\n}\n\n";

	

	return str;
}
