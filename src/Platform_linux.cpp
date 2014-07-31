#include "Platform.hpp"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

// TODO Rename shit names to more appropriate ones. -- nyanpasu

bool gt::Platform::checkDirExist(string dir)
{
	struct stat st;
	return stat(dir.c_str(), &st) == 0; //stat() returns 0 if the dir exist
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
			
	bool dirtyT = false, dirtyM = false;
			
	ifstream file;
	if(torrent)
	{
		file.open(getHomeDir() + ".local/share/applications/gtorrentt.desktop");
		if(file.is_open())
		{
			string tmp;
			for(int i = 0; i < 6; ++i) getline(file, tmp);
			dirtyT = (tmp != string("Exec=") + penis + " %F\n");
		}
		else
			dirtyT = true;
		file.close();
	}

	if(magnet)
	{
		file.open(getHomeDir() + ".local/share/applications/gtorrentm.desktop");

		if(file.is_open())
		{
			string tmp;
			for(int i = 0; i < 6; ++i) getline(file, tmp);
			dirtyM = (tmp != string("Exec=") + penis + " %u\n");
		}
		else
			dirtyM = true;
		file.close();
	}

	ofstream TassFile(getHomeDir() + ".local/share/applications/gtorrentt.desktop");
	ofstream MassFile(getHomeDir() + ".local/share/applications/gtorrentm.desktop");

	string TassString = 
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
		"MimeType=application/x-bittorrent;\n"               +
		"Categories=Internet;Network;FileTransfer;P2P;GTK;\n";

	string MassString = 
		string("[Desktop Entry]\n")                          +
		"Version=1.0\n"                                      +
		"Encoding=UTF-8\n"                                   + 
		"Name=gTorrent\n"                                    +
		"GenericName=BitTorrent Client\n"                    +
		"Comment=Share files over BitTorrent\n"              +
		"Exec=" + penis + " %u\n"                            +
		"Icon=gtorrent.png\n"                                +
		"Terminal=false\n"                                   +
		"Type=Application\n"                                 +
		"MimeType=x-scheme-handler/magnet;\n;"               +
		"Categories=Internet;Network;FileTransfer;P2P;GTK;\n";

	if(dirtyT)
		TassFile << TassString;
	if(dirtyM)
		MassFile << MassString;

	TassFile.close();
	MassFile.close();

	if(magnet) system("xdg-mime default gtorrentt.desktop application/x-bittorrent");
	if(torrent)system("xdg-mime default gtorrentm.desktop x-scheme-handler/magnet");
}

bool gt::Platform::sharedDataEnabled()
{
	return checkDirExist("/tmp/bigfatdick");
}

static int fd; 
void gt::Platform::makeSharedFile()
{
	if(mkfifo("/tmp/bigfatdick", 0755) == -1) throw "ABORT ABORT ABORT";
	fd = open("/tmp/bigfatdick", O_RDONLY | O_NONBLOCK); // TODO: use streams
}

#include "Log.hpp"
void gt::Platform::writeSharedData(string info)
{
	// I used write here but it didn't work.
	ofstream huehue("/tmp/bigfatdick");
	huehue << info << endl;
	huehue.close();
}

string gt::Platform::readSharedData()
{
	string tmp;
	char temp = '\0';
	while(read(fd, &temp, 1) && temp != '\n') tmp += temp;;
	return tmp;
}

void gt::Platform::disableSharedData()
{
	remove("/tmp/bigfatdick");
}

void gt::Platform::openTorrent(shared_ptr<gt::Torrent> t)
{
	auto files = t->getInfo()->files();
//	Glib::RefPtr<Gio::AppInfo> fileHandler = Gio::AppInfo::create_from_commandline("xdg-open", "If I knew I wouldn't ask, you turbonerd.", Gio::APP_INFO_CREATE_SUPPORTS_URIS);
	string path = t->getSavePath() + '/' + t->getInfo()->file_at(0).path;

	if (files.num_files() > 1) // if there's more than a file, we open the containing folder
		path = path.substr(0, path.find_last_of('/'));

//	Glib::RefPtr<Gio::File> thingtoopen = Gio::File::create_for_path(path);
//	fileHandler->launch(thingtoopen);
	system(string(string("xdg-open \'") + path + "\'").c_str());
}
