#include "Platform.hpp"
#include "Torrent.hpp"
#include "Log.hpp"

#include <iostream>
#include <cstdlib>

#include <windows.h>

// TODO Make sure any of this works. Fix all the pipe garbage (which is still a problem in Platform_linux.cpp) Figure
// out how to do the file association garbage using the registry.

// This will be renamed when Platform_linux.cpp and Platform.hpp change
// Actually checks if file/directory exists.
bool gt::Platform::checkDirExist(string file)
{
    DWORD f_attrib = GetFileAttributes(file.c_str())
    if(f_attrib != INVALID_FILE_ATTRIBUTES)
    {
        gt::Log::Debug(string(file + " exists,").c_str());
        return true;
    }
    else
    {
        gt::Log::Debug(string(file + " doesn't exist,").c_str());
        return false;
    }
}

string gt::Platform::getDefaultSavePath()
{
    gt::Log::Debug(string(getHomeDir() + "Downloads\\ is the default save path").c_str());
    return getHomeDir() + "Downloads";
}

string getDefaultConfigPath()
{
    return string(getenv("APPDATA")) + "\\gtorrent";
}

char gt::Platform::getFileSeparator()
{
    return '\\';
}

string getHomeDir()
{
    return string(getenv("HOMEDRIVE")) + string(getenv("HOMEPATH")) + '\\';
}

string getTempDir()
{
    return string(getenv("TMP")) + '\\';
}

int gt::Platform::makeDir(string dir, int mode)
{
    // TODO Actually use mode? Who really cares about perms in windows?
    return CreateDirectory(dir);
}

string gt::Platform::getExecutablePath()
{
    char ExecutablePath[4096] = { 0 };
    GetModuleFilename(NULL, ExecutablePath, 4096);

    return string(ExecutablePath);
}

void gt::Platform::associate(bool magnet, bool torrent)
{
    // TODO Figure out how to do registry keys and stuff.
    if(torrent)
    {
        
    }
    if(magnet)
    {
        
    }
}

void *ld = nullptr, *fd = nullptr;
bool gt::Platform::processIsUnique()
{
    if(ld == nullptr)
    {
        gt::Log::Debug("The lock wasn't ready, retrying...");
        if(!checkDirExist(getDefaultConfigPath()))
            makeDir(getDefaultConfigPath(), 0755);
        ld = CreateFile(string(getDefaultConfigPath() + "gtorrent.lock").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL);
        return processIsUnique();
    }
    int state = LockFile(ld);
    if(state == 0)
        gt::Log::Debug("Process is unique");
    if(state)
        gt::Log::Debug("Process is not unique");
    return state == 0;
}

void gt::Platform::makeSharedFile()
{
    if(processIsUnique() && !checkDirExist(getTempDir() + "gfeed")
        if(CreateNamedPipe(getTempDir() + "gfeed") == INVALID_HANDLE_VALUE)
            throw runtime_error("Couldn't create pipe! Check your permissions or if " + string(getenv("TMP") + "\\gfeed exists"));
    fd = CreateFile((getTempDir() + "gfeed").c_str(), GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL);
    if(fd == INVALID_HANDLE_VALUE)
        throw runtime_error("Couldn't open pipe");
    if(ld == INVALID_HANDLE_VALUE)
        ld = CreateFile((getTempDir() + "gtorrent.lock").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL);
    if(ld == INVALID_HANDLE_VALUE)
        throw runtime_error("Couldn't open pipe");
    processIsUnique();
}

void gt::Platform::writeSharedData(string info)
{
    std::ofstream file(getTempDir() + "gfeed");
    file << info << endl;
    file.close();
}

string gt::Platform::readSharedData()
{
    std::string sharedData;
    char tmp = '\0';
    while(read(fd, &tmp, 1) && tmp != '\n')
        sharedData += tmp;
    return sharedData;
}

void gt::Platform::disableSharedData()
{
    // Just copying the quality code from Platform_linux
    remove(getTempDir() + "gfeed");
}

void gt::Platform::openTorrent(shared_ptr<gt::Torrent> t)
{
    auto files = t->getInfo()->files();
    string path = t->getSavePath() + '\\' + t->getInfo()->file_at(0).path;

    if(files.num_files() > 1)
        path = path.substr(0, path.find_last_of('\\'));

    // TODO Find out what the fuck xdg-open does because fuck the guy who made the system call in Platform_linux
}
