#include "Platform.hpp"

bool gt::Platform::checkDirExist(string dir)
{
	return true; // Fake it till you make it -- nyanpasu
}

string gt::Platform::getDefaultSavePath()
{
	char *savedrive = getenv("HOMEDRIVE");
	char *savepath = getenv("HOMEPATH");
	return savepath == NULL ? string("") : string(savedrive) + string(savepath) + "/Downloads";
}
