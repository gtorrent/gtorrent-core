#pragma once
#include <string>
#include <sys/stat.h>

using namespace std;

namespace gt
{
	namespace Platform
	{
		// TODO Better names needed. Suggestions welcome.
		string getDefaultSavePath();
		string getDefaultConfigPath();
		string getHomeDir();
		int makeDir(std::string, mode_t);
		bool checkDirExist(string);
	}
}
