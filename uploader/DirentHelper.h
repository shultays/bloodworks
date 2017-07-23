#pragma once

#include "tools.h"
#include "dirent.h"
#include <string>
#include <vector>
#include <algorithm>
#include <functional>


namespace DirentHelper
{
	class File
	{
	public:
		std::string folder;
		std::string file;

		bool isTypeOf(const std::string& type) const
		{
			size_t pos = file.find_last_of(".");
			if (pos != std::string::npos && file.substr(pos + 1) == type)
			{
				return true;
			}
			return false;
		}
	};

	class Folder
	{
		DIR* openDir()
		{
			DIR *dir;
			dir = opendir(name.c_str());
			if (dir == nullptr)
			{
				printf("Folder '%s' does not exists\n", name.c_str());
			}
			return dir;
		}
		void closeDir(DIR *dir)
		{
			if (dir != nullptr)
			{
				closedir(dir);
			}
		}
	public:
		std::string name;
		Folder(const std::string& name)
		{
			this->name = name;
			fixFolderPath(this->name);
		}
		~Folder()
		{
		}

		std::vector<File> getAllFiles(bool recursive = false)
		{
			std::vector<File> files;
			appendAllFiles(files, recursive);
			return files;
		}

		void appendAllFiles(std::vector<File>& files, bool recursive = false)
		{
			std::function<void(File&)> func = [&](File &file) {files.push_back(file); };
			runOnEachFile(func, recursive);

		}


		void runOnEachFile(std::function<void(File&)>& func, bool recursive = false)
		{
			struct dirent *ent;
			DIR *dir = openDir();

			while ((ent = readdir(dir)) != NULL)
			{
				if (ent->d_name[0] != '.')
				{
					if (S_ISREG(ent->d_type))
					{
						File file;
						file.folder = name;
						file.file = ent->d_name;
						func(file);
					}
					else if (recursive && S_ISDIR(ent->d_type))
					{
						Folder f(name + ent->d_name);
						f.runOnEachFile(func, recursive);
					}
				}
			}

			closeDir(dir);
		}
	};

}
