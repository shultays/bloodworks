#pragma once

#include "cPackHelper.h"
#include "windows.h"
#include "DirentHelper.h"
#include "cTools.h"
#include <vector>
#include <iostream>
#include <string>

class cPackHelper
{
	static int getInt(FILE *fin)
	{
		int n;
		fread(&n, sizeof(int), 1, fin);
		return n;
	}

	static void putInt(FILE *fout, int i)
	{
		fwrite(&i, sizeof(int), 1, fout);
	}

	static std::string getStr(FILE *fin)
	{
		int n = getInt(fin);
		cVector<char> buff(n);
		fread(&buff[0], sizeof(char), n, fin);
		return std::string(&buff[0], n);
	}

	static void putStr(FILE *fout, const std::string& str)
	{
		int size = (int)str.size();
		putInt(fout, size);
		fwrite(str.c_str(), sizeof(char), size, fout);
	}

	static bool isTypeCorrect(const DirentHelper::File& f)
	{
		return f.isTypeOf("png") || f.isTypeOf("json") || f.isTypeOf("ogg") || f.isTypeOf("lua");
	}
public:
	static bool cPackHelper::packFolder(const std::string& pathUnfixed, const std::string& file, bool checkTypes = true)
	{
		std::string folderPath = pathUnfixed;
		fixFolderPath(folderPath);
		DirentHelper::Folder folder(folderPath);
		cVector<DirentHelper::File> files = folder.getAllFiles(true);

		if (files.size() > 0)
		{
			bool allValid = true;
			for (auto& f : files)
			{
				if (isTypeCorrect(f) == false)
				{
					allValid = false;
					std::cout << "Invalid file type : " << f.folder + f.file << std::endl;
				}
				else
				{
					//std::cout << f.folder + f.file << std::endl;
				}
			}
			if (allValid)
			{
				FILE *fout;
				fopen_s(&fout, file.c_str(), "wb");
				cVector<long> pos;
				pos.resize(files.size());
				putInt(fout, 0);
				putInt(fout, 0);
				putInt(fout, files.size());
				for (int i = 0; i < files.size(); i++)
				{
					auto& f = files[i];
					std::string path = f.folder + f.file;
					fixFilePath(path);

					path = path.substr(folderPath.size());

					putStr(fout, path);
					pos[i] = ftell(fout);
					putInt(fout, 0);
				}

				int headerSize = ftell(fout);
				fseek(fout, sizeof(int), SEEK_SET);
				fwrite(&headerSize, sizeof(int), 1, fout);
				fseek(fout, 0, SEEK_END);

				for (int i = 0; i < files.size(); i++)
				{
					auto& f = files[i];
					FILE *fin;
					fopen_s(&fin, (f.folder + f.file).c_str(), "rb");

					char buf[BUFSIZ];
					size_t total = 0;
					size_t n;
					while ((n = fread(buf, 1, sizeof(buf), fin)) > 0)
					{
						total += n;
						if (fwrite(buf, 1, n, fout) != n)
							abort();
						if (ferror(fout))
							abort();
					}
					fclose(fin);

					fseek(fout, pos[i], SEEK_SET);
					putInt(fout, (int)total);
					fseek(fout, 0, SEEK_END);
				}

				int fileSize = ftell(fout);
				fseek(fout, 0, SEEK_SET);
				fwrite(&fileSize, sizeof(int), 1, fout);

				fclose(fout);
				return true;
			}
		}
		return false;
	}

	static void cPackHelper::unpackFile(const std::string& f, const std::string& folder)
	{
		cVector<std::string> names;
		cVector<int> sizes;
		FILE *fin;
		fopen_s(&fin, f.c_str(), "rb");
		int totalSize = getInt(fin);
		int headerSize = getInt(fin);
		int fileCount = getInt(fin);
		for (int i = 0; i < fileCount; i++)
		{
			std::string s = getStr(fin);
			int len;
			len = getInt(fin);
			names.push_back(s);
			sizes.push_back(len);
		}
		for (int i = 0; i < fileCount; i++)
		{
			std::string s = names[i];
			int len = sizes[i];

			std::string output = folder + "/" + s;
			fixFilePath(output);

			for (int n = 3; n < output.size(); n++)
			{
				if (output[n] == '/')
				{
					std::string folder = output.substr(0, n);
					CreateDirectoryA(folder.c_str(), NULL);
				}
			}

			FILE *fout;
			fopen_s(&fout, output.c_str(), "wb");

			while (len > 0)
			{
				char buf[BUFSIZ];
				int read = min(len, (int)sizeof(buf));
				size_t n = fread(buf, 1, read, fin);
				fwrite(buf, 1, n, fout);
				len -= read;
			}

			fclose(fout);

		}
		fclose(fin);
	}

	static int cPackHelper::deleteFolder(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories /*= true*/, bool deleteRoot = true)
	{
		bool            bSubdirectory = false;       // Flag, indicating whether
													 // subdirectories have been found
		HANDLE          hFile;                       // Handle to directory
		std::string     strFilePath;                 // Filepath
		std::string     strPattern;                  // Pattern
		WIN32_FIND_DATAA FileInformation;             // File information


		strPattern = refcstrRootDirectory + "\\*.*";
		hFile = ::FindFirstFileA(strPattern.c_str(), &FileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FileInformation.cFileName[0] != '.')
				{
					strFilePath.erase();
					strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

					if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (bDeleteSubdirectories)
						{
							// Delete subdirectory
							int iRC = deleteFolder(strFilePath, bDeleteSubdirectories, true);
							if (iRC)
								return iRC;
						}
						else
							bSubdirectory = true;
					}
					else
					{
						// Set file attributes
						if (::SetFileAttributesA(strFilePath.c_str(),
							FILE_ATTRIBUTE_NORMAL) == FALSE)
							return ::GetLastError();

						// Delete file
						if (::DeleteFileA(strFilePath.c_str()) == FALSE)
							return ::GetLastError();
					}
				}
			} while (::FindNextFileA(hFile, &FileInformation) == TRUE);

			// Close handle
			::FindClose(hFile);

			if (deleteRoot)
			{
				DWORD dwError = ::GetLastError();
				if (dwError != ERROR_NO_MORE_FILES)
					return dwError;
				else
				{
					if (!bSubdirectory)
					{
						// Set directory attributes
						if (::SetFileAttributesA(refcstrRootDirectory.c_str(),
							FILE_ATTRIBUTE_NORMAL) == FALSE)
							return ::GetLastError();

						// Delete directory
						if (::RemoveDirectoryA(refcstrRootDirectory.c_str()) == FALSE)
							return ::GetLastError();
					}
				}
			}
		}

		return 0;
	}

	static int cPackHelper::deleteFile(const std::string& file)
	{
		if (::DeleteFileA(file.c_str()) == FALSE)
			return ::GetLastError();
		return 0;
	}

	static std::string cPackHelper::getFullPath(const std::string& file)
	{
		const int BUFSIZE = 512;
		DWORD  retval = 0;
		char  buffer[BUFSIZE];
		buffer[0] = '\0';

		GetFullPathNameA(file.c_str(),
			BUFSIZE,
			buffer,
			NULL);

		return std::string(buffer);
	}
};