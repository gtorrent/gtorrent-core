#include "Platform.hpp"
#include "Torrent.hpp"
#include "Log.hpp"

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
	int state = stat(dir.c_str(), &st);
	if(state == 0)
		gt::Log::Debug(string(dir + " exists,").c_str());
	if(state != 0)
		gt::Log::Debug(string(dir + " deoesn't exists,").c_str());
	return state == 0; //stat() returns 0 if the dir exist
}

string gt::Platform::getDefaultSavePath()
{
	// TODO Use XDG_DOWNLOAD or whatever it's called
	gt::Log::Debug(string(getHomeDir() + "Downloads/ is the default save path").c_str());
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

	char ExecutablePath[4096] = { 0 };
	readlink("/proc/self/exe", ExecutablePath, 4096);

	bool dirtyT = false, dirtyM = false;
			
        std::ifstream file;
	if(torrent)
	{
		file.open(getHomeDir() + ".local/share/applications/gtorrentt.desktop");
		if(file.is_open())
		{
			string tmp;
			for(int i = 0; i < 6; ++i) getline(file, tmp);
			dirtyT = (tmp != string("Exec=") + ExecutablePath + " %F\n");
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
			dirtyM = (tmp != string("Exec=") + ExecutablePath + " %u\n");
		}
		else
			dirtyM = true;
		file.close();
	}

        std::ofstream TassFile(getHomeDir() + ".local/share/applications/gtorrentt.desktop");
        std::ofstream MassFile(getHomeDir() + ".local/share/applications/gtorrentm.desktop");

	string TassString = 
		string("[Desktop Entry]\n")							 +
		"Version=1.0\n"										 +
		"Encoding=UTF-8\n"									 + 
		"Name=gTorrent\n"									 +
		"GenericName=BitTorrent Client\n"					 +
		"Comment=Share files over BitTorrent\n"				 +
		"Exec=" + ExecutablePath + " %F\n"					 +
		"Icon=gtorrent.png\n"								 +
		"Terminal=false\n"									 +
		"Type=Application\n"								 +
		"MimeType=application/x-bittorrent;\n"				 +
		"Categories=Internet;Network;FileTransfer;P2P;GTK;\n";

	string MassString = 
		string("[Desktop Entry]\n")							 +
		"Version=1.0\n"										 +
		"Encoding=UTF-8\n"									 + 
		"Name=gTorrent\n"									 +
		"GenericName=BitTorrent Client\n"					 +
		"Comment=Share files over BitTorrent\n"				 +
		"Exec=" + ExecutablePath + " %u\n"					 +
		"Icon=gtorrent.png\n"								 +
		"Terminal=false\n"									 +
		"Type=Application\n"								 +
		"MimeType=x-scheme-handler/magnet;\n;"				 +
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
	return checkDirExist("/tmp/gfeed");
}

int fd = -1, ld = -1;
bool gt::Platform::processIsUnique()
{
	if(ld == -1)
	{
		gt::Log::Debug("The lock wasn't ready, retrying...");
		ld = open(string(getDefaultConfigPath() + "gtorrent.lock").c_str(), O_CREAT | O_RDWR, 0600);
		return processIsUnique();
	}
	struct flock fl = { 0 };
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	int state = fcntl(ld, F_SETLK, &fl);
	if(state == 0)
		gt::Log::Debug("Process is unique");
	if(state)
		gt::Log::Debug("Process is not unique");
	return state == 0;
}

void gt::Platform::makeSharedFile()
{
	if(processIsUnique())
		if(mkfifo("/tmp/gfeed", 0755) == -1)
			throw runtime_error("Couldn't create pipe! Check your permissions or if /tmp/gfeed exists");
	fd = open("/tmp/gfeed", O_RDONLY | O_NONBLOCK); // TODO: use streams
	if(fd == -1)
		throw runtime_error("Couldn't open pipe");
	if(ld == -1)
		ld = open("/var/lock/gtorrent.lock", O_CREAT | O_RDONLY, 0600);
	if(ld == -1)
		throw runtime_error("Couldn't open pipe");
	processIsUnique(); // a call here to lock the file
}

void gt::Platform::writeSharedData(string info)
{
	// I used write here but it didn't work.
        std::ofstream file("/tmp/gfeed");
	file << info << endl;
	file.close();
}

string gt::Platform::readSharedData()
{
	string stringThatContainsTheLineWeAreAboutToReadInto;
	char temporaryCharacterThatContainTheCharactersWeHaveReadFromThePipe = '\0';
	while(read(fd, &temporaryCharacterThatContainTheCharactersWeHaveReadFromThePipe, 1) && 
		  temporaryCharacterThatContainTheCharactersWeHaveReadFromThePipe != '\n') 
		stringThatContainsTheLineWeAreAboutToReadInto += temporaryCharacterThatContainTheCharactersWeHaveReadFromThePipe;
	return stringThatContainsTheLineWeAreAboutToReadInto;
}

void gt::Platform::disableSharedData()
{
	// Thread safe high quality standard compliant function.
	remove("/tmp/gfeed"); // Remove the file from the file system according to the C-style string at the address passed as it's first parameter, it is compliant with C89, C98 and POSIX1 standard from 2001
	// Its definition is located in the stdio.h header, or cstdio under c++.
}

void gt::Platform::openTorrent(shared_ptr<gt::Torrent> t)
{
	auto files = t->getInfo()->files();
	string path = t->getSavePath() + '/' + t->getInfo()->file_at(0).path;

	if (files.num_files() > 1) // if there's more than a file, we open the containing folder
		path = path.substr(0, path.find_last_of('/'));

	// HURR system() IS BAD BECAUSE IT'S NOT USED TO MAKE PORTABLE CODE, pls refer to the filename, if you're expecting anything portable here you've come to the wrong place.
	system(string(string("xdg-open \'") + path + "\'").c_str()); // Either use system or fork and exec with the xdg binary it's literraly the same shit, or even worst, link with even more libs, pick your poison
}
