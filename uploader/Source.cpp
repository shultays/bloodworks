#include "tools.h"
#include <string>
#include <algorithm>

void fixFilePath(std::string& path)
{
	std::replace(path.begin(), path.end(), '\\', '/');
	if (path[0] == '/')
	{
		path = path.substr(1);
	}
}

void fixFolderPath(std::string& path)
{
	fixFilePath(path);
	if (path[path.size() - 1] != '/')
	{
		path = path + '/';
	}
}

