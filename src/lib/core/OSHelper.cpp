#include "OSHelper.hpp"

std::string OSHelper::getBasename(std::string strPath)
{
	size_t iLastSeparator = 0;
    return strPath.substr((iLastSeparator = strPath.find_last_of("\\")) != std::string::npos ? iLastSeparator + 1 : 0, strPath.size() - strPath.find_last_of("."));
}