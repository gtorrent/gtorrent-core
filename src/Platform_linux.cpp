#include "Platform.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

bool gt::Platform::checkDirExist(string dir)
{
	struct stat st;
	if(stat(dir.c_str(), &st)) //stat() returns 0 if the dir exist
		return false;

	return true;
}

string gt::Platform::getDefaultSavePath()
{
	return getHomeDir() + "/Downloads";
}

string gt::Platform::getDefaultConfigPath()
{
	string config_home = getenv("XDG_CONFIG_HOME");
	if (config_home.length() == 0)
		config_home = ".config/";

	return getHomeDir() + config_home;
}

string gt::Platform::getHomeDir()
{
	struct passwd *pw = getpwuid(getuid());
	string dir = pw->pw_dir;
	return dir + "/";
}
