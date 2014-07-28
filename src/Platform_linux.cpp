#ifndef _WIN32
#include "Platform.hpp"

#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// TODO Rename shit names to more appropriate ones. -- nyanpasu

bool gt::Platform::checkDirExist(string dir)
{
	struct stat st;
	if(stat(dir.c_str(), &st)) //stat() returns 0 if the dir exist
	{
		return false;
	}

	return true;
}

string gt::Platform::getDefaultSavePath()
{
	// TODO Use XDG_DOWNLOAD or whatever it's called
	return getHomeDir() + "Downloads/";
}

string gt::Platform::getDefaultConfigPath()
{
	const char *c = getenv("XDG_CONFIG_HOME");
	string config_home;

	if (c == nullptr)
		config_home = getHomeDir() + ".config";
	else
		config_home = c;

	config_home += "/gtorrent/";

	return config_home;
}

char gt::Platform::getFileSeparator()
{
	return '/';
}

string gt::Platform::getHomeDir()
{
	string dir = getenv("HOME");
	return dir + "/";
}

int gt::Platform::makeDir(std::string dir, mode_t mode)
{
	return mkdir(dir.c_str(), mode);
}
#endif