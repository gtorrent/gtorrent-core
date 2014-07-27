#include "Platform.hpp"

bool gt::Core::isMagnetLink(string const& url)
{
	const string prefix = "magnet:";
	return url.compare(0, prefix.length(), prefix) == 0;
}

bool gt::Core::checkDirExist(string dir)
{
	return true; // Fake it till you make it -- nyanpasu
}

string gt::Core::getDefaultSavePath()
{
	char *savedrive = getenv("HOMEDRIVE");
	char *savepath = getenv("HOMEPATH");
	return savepath == NULL ? string("") : string(savedrive) + string(savepath) + "/Downloads";
}
