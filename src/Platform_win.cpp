#include "Platform.hpp"
#include "Torrent.hpp"
#include "Log.hpp"

#include <iostream>
#include <cstdlib>
#include <sstream>

#include <windows.h>

// TODO Make sure any of this works. Fix all the pipe garbage (which is still a problem in Platform_linux.cpp) Figure
// out how to do the file association garbage using the registry.

// This will be renamed when Platform_linux.cpp and Platform.hpp change
// Actually checks if file/directory exists.
bool gt::Platform::checkDirExist(std::string file)
{
    DWORD f_attrib = GetFileAttributes(file.c_str());
    if(f_attrib != INVALID_FILE_ATTRIBUTES)
    {
        gt::Log::Debug(std::string(file + " exists,").c_str());
        return true;
    }
    else
    {
        gt::Log::Debug(std::string(file + " doesn't exist,").c_str());
        return false;
    }
}

std::string gt::Platform::getDefaultSavePath()
{
    gt::Log::Debug(std::string(getHomeDir() + "Downloads\\ is the default save path").c_str());
    return getHomeDir() + "Downloads";
}

std::string gt::Platform::getDefaultConfigPath()
{
    return std::string(getenv("APPDATA")) + "\\gtorrent\\";
}

char gt::Platform::getFileSeparator()
{
    return '\\';
}

std::string gt::Platform::getHomeDir()
{
    return std::string(getenv("HOMEDRIVE")) + std::string(getenv("HOMEPATH")) + '\\';
}

std::string getTempDir()
{
    return std::string(getenv("TMP")) + '\\';
}

int gt::Platform::makeDir(std::string dir, unsigned mode)
{
    // TODO Actually use mode? Who really cares about perms in windows?
    return CreateDirectory(dir.c_str(), NULL);
}

std::string gt::Platform::getExecutablePath()
{
    // A path is usually not 4096 chars long, right?
    char ExecutablePath[4096] = { 0 };
    GetModuleFileName(NULL, ExecutablePath, 4096);

    return std::string(ExecutablePath);
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
{//Currently locking doesn't work because the program can't delete the file at the end
	static bool isUnique=false;
	if(isUnique)
	{
		gt::Log::Debug("Process is unique");
		return true;
	}
    if(ld == nullptr)
    {
        gt::Log::Debug("The lock wasn't ready, retrying...");
        if(!checkDirExist(getDefaultConfigPath()))
            makeDir(getDefaultConfigPath(), 0755);
        ld = CreateFile(std::string(getDefaultConfigPath() + "gtorrent.lock").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        return processIsUnique();
    }
    int state = LockFile(ld, 0, 0, 1, 0);
    if(state != 0) //Zero if lock succeeds, Linux != Windows
        gt::Log::Debug("Process is unique");
    if(state ==0)
        gt::Log::Debug("Process is not unique");
    isUnique = state != 0;
	return isUnique;
}

void gt::Platform::makeSharedFile()
{
    if(processIsUnique() && !checkDirExist("\\\\.\\pipe\\gfeed"))
        if(CreateNamedPipe("\\\\.\\pipe\\gfeed", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, 1, 1, 1, 0, NULL) == INVALID_HANDLE_VALUE)
            throw std::runtime_error("Couldn't create pipe! Check your permissions or if \\\\.\\pipe\\gfeed exists");
    fd = CreateFile("\\\\.\\pipe\\gfeed", GENERIC_READ | GENERIC_WRITE , 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fd == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Couldn't open feed pipe");
    if(ld == INVALID_HANDLE_VALUE || ld == nullptr)
        ld = CreateFile(std::string(getDefaultConfigPath() + "gtorrent.lock").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if(ld == INVALID_HANDLE_VALUE || ld == nullptr)
        throw std::runtime_error("Couldn't open lock pipe");
    processIsUnique();
}

void gt::Platform::writeSharedData(std::string info)
{
	const char* tosend = info.c_str();
	DWORD bytesWritten;
	std::ostringstream exceptionStream;
    if (! WriteFile(fd, tosend, sizeof(tosend), &bytesWritten, NULL))
	{
		exceptionStream<<"Write to pipe failed with "<<bytesWritten<<" of "<<sizeof(tosend)<<" in "<<tosend;
		throw std::runtime_error(exceptionStream.str());
	}
}

std::string gt::Platform::readSharedData()
{
	DWORD pipemode = PIPE_READMODE_MESSAGE | PIPE_NOWAIT;
	SetNamedPipeHandleState(fd,&pipemode, NULL, NULL);
    std::string sharedData;
    char tmp[2];
	DWORD cbRead=sizeof(char);
	if (fd == INVALID_HANDLE_VALUE || fd == nullptr)
		gt::Log::Debug("fd is invalid handle");
    while(ReadFile(fd, tmp, sizeof(char), &cbRead, NULL) && ( tmp[cbRead/sizeof(char)]='\0' ) && tmp[1] != '\n') //IMPORTANT: that is assignment, not comparison DO NOT CHANGE = TO == 
        sharedData += std::string(tmp);
	return sharedData;
}

void gt::Platform::disableSharedData()
{
	//Windows named pipes should delete when there's nobody with access
	gt::Log::Debug("Deleting lock file.");//Doesn't work, not called when program exits
	UnlockFile(ld, 0, 0, 1, 0);
	DeleteFile(std::string(getDefaultConfigPath() + "gtorrent.lock").c_str());
}

void gt::Platform::openTorrent(std::shared_ptr<gt::Torrent> t)
{
    auto files = t->getInfo()->files();
    std::string path = t->getSavePath() + '\\' + t->getInfo()->file_at(0).path;

    if(files.num_files() > 1)
        path = path.substr(0, path.find_last_of('\\'));
    // Windows actually does this better than linux :o
    ShellExecute(0, 0, path.c_str(), 0, 0, SW_SHOW);
}
