#ifdef _WIN32
#include "Platform.hpp"

#include <Windows.h> // General Windows API
#include <ShlObj.h> // SHGetKnownFolderPath
#include <direct.h> // _mkdir
#include <sstream>


// Convert a wide Unicode string to a Windows ANSI codepage string.
//
// This conversion will be lossy if the wstring contains non-Western Unicode characters,
// resulting in it not working with the Windows file system API.
//
// TODO:
// This method is for compatiblity with the Platform.hpp interface definitions
// which only use std::string. We should be using std::wstring for dir/file paths
// for Windows unicode support.
string wideToMultiByteString(const wstring &wstr)
{
	int sz = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
	string strTo(sz, 0);
	WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], sz, nullptr, nullptr);
	return strTo;
}

// Does NOT return a trailing backslash.
string getKnownFolder(REFKNOWNFOLDERID rfid)
{
	wchar_t* dirPath = nullptr;

	// We are required to free `dirPath` ourselves using `CoTaskMemFree`.
	// So we will copy the contents into a string and then delete it.
	SHGetKnownFolderPath(rfid, 0, nullptr, &dirPath);

	wstringstream ss;
	ss << dirPath;

	CoTaskMemFree(static_cast<void*>(dirPath));

	return wideToMultiByteString(ss.str());
}



string gt::Platform::getDefaultSavePath()
{
	// Example: C:\Users\UserName\Downloads\
	// Be consistent - always return a trailing file separator.
	return getKnownFolder(FOLDERID_Downloads) + getFileSeparator();
}

string gt::Platform::getDefaultConfigPath()
{
	// Example: C:\Users\UserName\.gtorrent\
	// Be consistent - always return a trailing file separator.
	return getHomeDir() + string(".gtorrent") + getFileSeparator();
}

string gt::Platform::getHomeDir()
{
	// Example: C:\Users\UserName\
	// Be consistent - always return a trailing file separator.
	return getKnownFolder(FOLDERID_Profile) + getFileSeparator();
}

char gt::Platform::getFileSeparator()
{
	return '\\';
}

int gt::Platform::makeDir(std::string dir, mode_t mode)
{
	// TODO: any way to emulate UNIX permissions? Mode is just ignored atm.
	return _mkdir(dir.c_str());
}

// Works with and without a leading file separator.
bool gt::Platform::checkDirExist(string dir)
{
	DWORD dwAttrib = GetFileAttributesA(dir.c_str());

	// TODO: this returns true for both files and directories, as the logic in core
	// seemed to require it.
	return (dwAttrib != INVALID_FILE_ATTRIBUTES);

	// TODO: this returns true only for directories. Uncomment this once logic has been sorted out.
	// return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#endif
