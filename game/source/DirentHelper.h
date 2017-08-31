#pragma once

#include "dirent.h"
#include "cTools.h"
#include <string>
#include <vector>
#include <functional>

namespace DirentHelper
{
	class File
	{
	public:
		std::string folder;
		std::string file;
		File(){}
		File(const std::string folder, const std::string file)
		{
			this->folder = folder;
			fixFolderPath(this->folder);
			this->file = file;
			fixFilePath(this->file);
		}
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
		std::string name;
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
		Folder(const std::string& name)
		{
			this->name = name;
			fixFolderPath(this->name);
		}
		~Folder()
		{
		}

		bool exists() const
		{
			DIR *dir;
			dir = opendir(name.c_str());
			if (dir == nullptr)
			{
				closedir(dir);
				return true;
			}
			return false;
		}

		const std::string getPath() const
		{
			return name;
		}

		cVector<Folder> getSubFolders()
		{
			cVector<Folder> folders;
			struct dirent *ent;
			DIR *dir = openDir();
			while ((ent = readdir(dir)) != NULL)
			{
				if (ent->d_name[0] != '.')
				{
					if (S_ISREG(ent->d_type))
					{
					}
					else if (S_ISDIR(ent->d_type))
					{
						std::string s = name + ent->d_name;
						Folder f(s);
						folders.push_back(f);
					}
				}
			}
			closeDir(dir);
			return folders;
		}

		cVector<File> getAllFiles(bool recursive = false)
		{
			cVector<File> files;
			appendAllFiles(files, recursive);
			return files;
		}

		void appendAllFiles(cVector<File>& files, bool recursive = false)
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
