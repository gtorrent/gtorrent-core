#include "Platform.hpp"

bool gt::Platform::checkDirExist(string dir)
{
	return true; // Fake it till you make it -- nyanpasu
}

string gt::Platform::getDefaultSavePath()
{
	char *savepath = getenv("HOME");
	return savepath == NULL ? string("") : string(savepath) + "/Downloads";
}
