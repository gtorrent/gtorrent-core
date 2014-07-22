#include <Utils.hpp>

namespace gt
{
	bool isMagnetLink(string const& url)
	{
		const string prefix = "magnet:";
		return url.compare(0, prefix.length(), prefix) == 0;
	}

	string getDefaultSavePath()
	{
#ifndef _WIN32
		char *savepath = getenv("HOME");
		return savepath == NULL ? string("") : string(savepath) + "/Downloads";
#else
		char *savedrive = getenv("HOMEDRIVE");
		char *savepath = getenv("HOMEPATH");
		return savepath == NULL ? string("") : string(savedrive) + string(savepath) + "/Downloads";
#endif
	}

}
