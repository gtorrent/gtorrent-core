#include "Platform.hpp"

// TODO Implement this shit. None of it is finished and I have no intention of learning
// how windows is supposed to do these. Feel free to volunteer your implementation.

bool gt::Platform::checkDirExist(string dir)
{
	return true; // Fake it till you make it -- nyanpasu
}

string gt::Platform::getDefaultSavePath()
{
	char *savepath = getenv("HOME");
	return savepath == NULL ? string("") : string(savepath) + "/Downloads";
}

char gt::Platform::getFileSeparator()
{
	return '\\';
}

string getDefaultConfigPath()
{
	return "C:\\windowsisshit";
}

string getHomeDir()
{
	return "C:\\";
}
