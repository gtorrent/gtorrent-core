#include "Config.hpp"
#include "Platform.hpp"
#include "Torrent.hpp"
#include "Log.hpp"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

std::string gt::Platform::getEnv(std::string env, std::string default_value)
{
	const char *c = getenv(env.c_str());
	return c ? c : Platform::getHomeDir() + default_value;
}

std::string gt::Platform::getHomeDir()
{
	return getenv("HOME");
}

std::string gt::Platform::getXdgSavePath()
{
	return gt::Platform::getEnv("XDG_DOWNLOAD_DIR", XDG_DEFAULT_DOWNLOAD_DIR);
}

std::string gt::Platform::getXdgConfigPath()
{
	return gt::Platform::getEnv("XDG_CONFIG_DIR", XDG_DEFAULT_CONFIG_HOME);
}

std::string gt::Platform::getDefaultConfigPath()
{
	return getXdgConfigPath() + CONFIG_GTORRENT_DEFAULT_HOME;
}

bool gt::Platform::dirExist(std::string dir)
{
	struct stat st;
	int state = stat(dir.c_str(), &st);
	int permissions = st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	if (permissions != 0755)
		chmod(dir.c_str(), 0755); //fix permissions if they are not good
	return state == 0; //stat() returns 0 if the dir exist
}

char gt::Platform::getFileSeparator()
{
	return '/';
}

int gt::Platform::makeDir(std::string dir, mode_t mode)
{
	return mkdir(dir.c_str(), mode);
}

std::string gt::Platform::getExecutablePath()
{
	char ExecutablePath[4096] = {0};
	readlink("/proc/self/exe", ExecutablePath, 4096);

	return std::string(ExecutablePath);
}

// TODO #126 association should be performed during installation
// All dat magic strings
// Use /usr/share/applications/ instead, like real programmers.
void gt::Platform::associate(bool magnet, bool torrent)
{
	// TODO A function that recursively creates directories in gt::Platform
	makeDir(getHomeDir() + "/.local", 0755);
	makeDir(getHomeDir() + "/.local/share", 0755);
	makeDir(getHomeDir() + "/.local/share/applications", 0755);

	bool dirtyT = false, dirtyM = false;

	std::ifstream file;
	if (torrent) {
		file.open(getHomeDir() + "/.local/share/applications/gtorrentt.desktop");
		if (file.is_open()) {
			std::string tmp;
			for (int i = 0; i < 6; ++i) getline(file, tmp);
			dirtyT = (tmp != std::string("Exec=") + getExecutablePath() + " %F\n");
		}
		else
			dirtyT = true;
		file.close();
	}

	if (magnet) {
		file.open(getHomeDir() + "/.local/share/applications/gtorrentm.desktop");

		if (file.is_open()) {
			std::string tmp;
			for (int i = 0; i < 6; ++i) getline(file, tmp);
			dirtyM = (tmp != std::string("Exec=") + getExecutablePath() + " %u\n");
		}
		else
			dirtyM = true;
		file.close();
	}

	std::ofstream TassFile(getHomeDir() + "/.local/share/applications/gtorrentt.desktop");
	std::ofstream MassFile(getHomeDir() + "/.local/share/applications/gtorrentm.desktop");

	std::string TassString =
		std::string("[Desktop Entry]\n") +
			"Version=1.0\n" +
			"Encoding=UTF-8\n" +
			"Name=gTorrent\n" +
			"GenericName=BitTorrent Client\n" +
			"Comment=Share files over BitTorrent\n" +
			"Exec=" + getExecutablePath() + " %F\n" +
			"Icon=gtorrent.png\n" +
			"Terminal=false\n" +
			"Type=Application\n" +
			"MimeType=application/x-bittorrent;\n" +
			"Categories=Internet;Network;FileTransfer;P2P;GTK;\n";

	std::string MassString =
		std::string("[Desktop Entry]\n") +
			"Version=1.0\n" +
			"Encoding=UTF-8\n" +
			"Name=gTorrent\n" +
			"GenericName=BitTorrent Client\n" +
			"Comment=Share files over BitTorrent\n" +
			"Exec=" + getExecutablePath() + " %u\n" +
			"Icon=gtorrent.png\n" +
			"Terminal=false\n" +
			"Type=Application\n" +
			"MimeType=x-scheme-handler/magnet;\n;" +
			"Categories=Internet;Network;FileTransfer;P2P;GTK;\n";

	if (dirtyT)
		TassFile << TassString;
	if (dirtyM)
		MassFile << MassString;

	TassFile.close();
	MassFile.close();

	if (torrent) system("xdg-mime default gtorrentt.desktop application/x-bittorrent");
	if (magnet) system("xdg-mime default gtorrentm.desktop x-scheme-handler/magnet");
}

static int fd = -1, ld = -2;

bool gt::Platform::processIsUnique()
{
	if (ld < -1) {
		gt::Log::Debug("The lock wasn't ready, retrying...");
		if (!dirExist(getDefaultConfigPath()))
			makeDir(getDefaultConfigPath(), 0755);
		ld = open(std::string(getDefaultConfigPath() + "gtorrent.lock").c_str(), O_CREAT | O_RDWR, 0600);
		return processIsUnique();
	}
	struct flock fl = {0};
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	int state = fcntl(ld, F_SETLK, &fl);
	if (state == 0)
		gt::Log::Debug("Process is unique");
	else
		gt::Log::Debug("Process is not unique");
	return state == 0;
}

void gt::Platform::makeSharedFile()
{
	if (processIsUnique() && !dirExist("/tmp/gfeed")) //If the pipe already exists we'll just use it
	if (mkfifo("/tmp/gfeed", 0755) == -1)
		throw std::runtime_error("Couldn't create pipe! Check your permissions or if /tmp/gfeed exists");
	fd = open("/tmp/gfeed", O_RDONLY | O_NONBLOCK); // TODO: use streams
	if (fd == -1)
		throw std::runtime_error("Couldn't open pipe");
	if (ld == -1)
		throw std::runtime_error("Couldn't lock");
	processIsUnique(); // a call here to lock the file
}

void gt::Platform::writeSharedData(std::string info)
{
	// I used write here but it didn't work.
	std::ofstream file("/tmp/gfeed");
	file << info << std::endl;
	file.close();
}

std::string gt::Platform::readSharedData()
{
	std::string sharedData;
	char tmp = '\0';
	while (read(fd, &tmp, 1) && tmp != '\n')
		sharedData += tmp;
	return sharedData;
}

// Thread safe high quality standard compliant function.
void gt::Platform::disableSharedData()
{
	remove("/tmp/gfeed"); // Remove the file from the file system according to the C-style string at the address passed as it's first parameter, it is compliant with C89, C98 and POSIX1 standard from 2001
	// Its definition is located in the stdio.h header, or cstdio under c++.
}

static std::string escape(std::string str)
{
	for (unsigned i = 0; i < str.size(); ++i)
		if (str[i] == '\'') str.insert((i += 3) - 3, "'\\'");
	return str;
}

void gt::Platform::openTorrent(std::shared_ptr<gt::Torrent> t, int index, bool folder)
{
	auto files = t->torrent_file()->files();
	std::string path;

	if (index == -1) //file to open wasn't specified, so if theres one file, we open it, or else we open the folder
	{
		path = t->getSavePath() + '/' + files.at(0).path;
		if (files.num_files() > 1) // if there's more than a file, and the file to open wasn't specified, we open the containing folder
			path = t->getSavePath() + '/' + files.name();
		path = escape(path);
	}
	else //We open the specified file, or its parent
	{
		path = t->getSavePath() + '/' + t->torrent_file()->file_at(index).path;
		if (folder)
			path = path.substr(0, path.find_last_of('/'));
		path = escape(path);
	}

	// HURR system() IS BAD BECAUSE IT'S NOT USED TO MAKE PORTABLE CODE, pls refer to the filename, if you're expecting anything portable here you've come to the wrong place.
	system(std::string(std::string("xdg-open \'") + path + "\'&").c_str()); // Either use system or fork and exec with the xdg binary it's literraly the same shit, or even worst, link with even more libs, pick your poison
}

