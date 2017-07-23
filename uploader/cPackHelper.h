#pragma once

#include <string>

class cPackHelper
{
public:

	static bool packFolder(const std::string& folder, const std::string& file, bool checkTypes = true);
	static void unpackFile(const std::string& file, const std::string& folder);

	static int deleteFolder(const std::string& path, bool recursive = true);
	static int deleteFile(const std::string& file);
	static std::string getFullPath(const std::string& file);
};