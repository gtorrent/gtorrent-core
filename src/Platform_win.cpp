#ifdef _WIN32
#include "Platform.hpp"

#include <cstdlib>

// TODO Implement this shit. None of it is finished and I have no intention of learning
// how windows is supposed to do these. Feel free to volunteer your implementation.

string gt::Platform::getDefaultSavePath()
{
	char *savepath = getenv("HOME");
	return savepath == NULL ? string("") : string(savepath) + "/Downloads";
}

string gt::Platform::getDefaultConfigPath()
{
	return "C:\\windowsisshit";
}

string gt::Platform::getHomeDir()
{
	string dir = getenv("USERPROFILE");
	return dir + "\\";
}

char gt::Platform::getFileSeparator()
{
	return '\\';
}

int gt::Platform::makeDir(std::string, mode_t mode = 0)
{
	return 0;
}

bool gt::Platform::checkDirExist(string dir)
{
	return true; // Fake it till you make it -- nyanpasu
}

#endif