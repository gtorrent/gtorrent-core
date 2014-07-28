#include "Platform.hpp"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// TODO Rename shit names to more appropriate ones. -- nyanpasu

bool gt::Platform::checkDirExist(string dir)
{
	struct stat st;
	return stat(dir.c_str(), &st) != 0; //stat() returns 0 if the dir exist
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

void gt::Platform::associate(bool magnet, bool torrent)
{
	makeDir(getHomeDir() + ".local", 0755);
	makeDir(getHomeDir() + ".local/share", 0755);
	makeDir(getHomeDir() + ".local/share/applications", 0755);

	char penis[4096] = { 0 };
	readlink("/proc/self/exe", penis, 4096);
			
	bool dirty = false;
			
	ifstream file(getHomeDir() + ".local/share/applications/gtorrent.desktop");
	if(file.is_open())
	{
		string tmp;
		for(int i = 0; i < 6; ++i) getline(file, tmp);
		dirty = (tmp != string("Exec=") + penis + " %F\n");
	}
	else
		dirty = true;
	file.close();

	ofstream assFile(getHomeDir() + ".local/share/applications/gtorrent.desktop");

	string assString = 
		string("[Desktop Entry]\n")                          +
		"Version=1.0\n"                                      +
		"Encoding=UTF-8\n"                                   + 
		"Name=gTorrent\n"                                    +
		"GenericName=BitTorrent Client\n"                    +
		"Comment=Share files over BitTorrent\n"              +
		"Exec=" + penis + " %F\n"                            +
		"Icon=gtorrent.png\n"                                +
		"Terminal=false\n"                                   +
		"Type=Application\n"                                 +
		"MimeType=application/x-bittorrent;"                 +
		"x-scheme-handler/magnet;\n"                         +
		"Categories=Internet;Network;FileTransfer;P2P;GTK;\n";

	if(dirty)
	{
		cout << "shits dirty" << endl;
		assFile << assString << endl;
	}
	assFile.close();
	if(magnet) system("xdg-mime default gtorrent.desktop application/x-bittorrent");
	if(torrent)system("xdg-mime default gtorrent.desktop x-scheme-handler/magnet");
}
