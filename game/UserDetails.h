#pragma once

#include <string>
#include "UserDetails.h"
#include "cPackHelper.h"
#include "json.h"

#include <fstream>
#include <algorithm>
#include <iostream>

#define CURL_STATICLIB 
#include <curl/curl.h>

#define USERFILE "uploader/user_details.txt"

class UserDetails
{
	std::string username;
	std::string password;

	bool valids[256];
	bool logined;

	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		std::string* str = (std::string*)userp;
		size_t realsize = size * nmemb;
		(*str).append((char*)contents, realsize);
		return realsize;
	}
public:
	UserDetails::UserDetails()
	{
		memset(valids, 0, sizeof(valids));
		for (char c = 'a'; c <= 'z'; c++)
		{
			valids[c] = true;
		}
		for (char c = 'A'; c <= 'Z'; c++)
		{
			valids[c] = true;
		}
		for (char c = '0'; c <= '9'; c++)
		{
			valids[c] = true;
		}
		valids[' '] = true;
		valids['-'] = true;
		valids['+'] = true;
		valids['.'] = true;
		valids['_'] = true;
		valids['='] = true;
		valids['@'] = true;
		valids['!'] = true;
		valids['?'] = true;
		valids[','] = true;
		valids[';'] = true;
		valids['~'] = true;
		valids['#'] = true;
		valids['('] = true;
		valids[')'] = true;
		valids['['] = true;
		valids[']'] = true;
		valids['{'] = true;
		valids['}'] = true;
		valids['*'] = true;
		valids['/'] = true;

		logined = false;

		std::ifstream file(USERFILE);
		if (file.is_open())
		{
			std::string username;
			std::string password;

			std::getline(file, username);
			std::getline(file, password);
			file.close();

			if (username.length() > 0 && password.length() > 0)
			{
				if (getInvalidCharsInUsername(username).size() > 0 || getInvalidCharsInPassword(password).size() > 0)
				{
					printf("Invalid username/password stored in file\n");
					std::ofstream file(USERFILE);
					file.close();
				}
				else
				{
					this->username = username;
					this->password = password;
				}
			}
		}
	}

	bool UserDetails::tryLogin()
	{
		return tryLogin(username, password);
	}

	bool UserDetails::tryLogin(const std::string& username, const std::string& password)
	{
		std::string readBuffer;
		if (logined == false && username.length() > 0 && password.length() > 0)
		{
			CURL *curl = curl_easy_init();
			if (curl)
			{
				struct curl_httppost *formpost = NULL;
				struct curl_httppost *lastptr = NULL;
				struct curl_slist *headerlist = NULL;

				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "username",
					CURLFORM_COPYCONTENTS, username.c_str(),
					CURLFORM_END);

				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "password",
					CURLFORM_COPYCONTENTS, password.c_str(),
					CURLFORM_END);

				curl_easy_setopt(curl, CURLOPT_URL, "http://bloodworks.enginmercan.com/login.php");
				curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				CURLcode res = curl_easy_perform(curl);
				if (res != CURLE_OK)
					fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));
				logined = readBuffer == "true";
				curl_easy_cleanup(curl);
				curl_formfree(formpost);
				curl_slist_free_all(headerlist);
			}

			if (logined)
			{
				std::ofstream file(USERFILE);
				file << username << std::endl;
				file << password << std::endl;
				file.close();
				this->username = username;
				this->password = password;
			}
			else
			{
				std::cout << "Error : " << readBuffer << std::endl;
			}
		}
		return logined;
	}

	const std::string UserDetails::getInvalidCharsInUsername(const std::string& username) const
	{
		std::string ret;
		bool buff[256];
		memset(buff, 0, sizeof(buff));
		for (char c : username)
		{
			if (valids[c] == false)
			{
				if (buff[c] == false)
				{
					buff[c] = true;
					ret += c;
				}
			}
		}
		std::sort(ret.begin(), ret.end());
		return ret;
	}

	const std::string UserDetails::getInvalidCharsInPassword(const std::string& password) const
	{
		return getInvalidCharsInUsername(password);
	}

	bool UserDetails::tryRegister(const std::string& username, const std::string& password)
	{
		logined = false;
		std::string readBuffer;
		if (logined == false && username.length() > 0 && password.length() > 0)
		{
			CURL *curl = curl_easy_init();
			if (curl)
			{
				struct curl_httppost *formpost = NULL;
				struct curl_httppost *lastptr = NULL;
				struct curl_slist *headerlist = NULL;

				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "username",
					CURLFORM_COPYCONTENTS, username.c_str(),
					CURLFORM_END);

				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "password",
					CURLFORM_COPYCONTENTS, password.c_str(),
					CURLFORM_END);

				curl_easy_setopt(curl, CURLOPT_URL, "http://bloodworks.enginmercan.com/register.php");
				curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				CURLcode res = curl_easy_perform(curl);
				if (res != CURLE_OK)
					fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));
				logined = readBuffer == "true";
				curl_easy_cleanup(curl);
				curl_formfree(formpost);
				curl_slist_free_all(headerlist);
			}

			if (logined)
			{
				std::ofstream file(USERFILE);
				file << username << std::endl;
				file << password << std::endl;
				file.close();
				this->username = username;
				this->password = password;
			}
			else
			{
				std::cout << "Error : " << readBuffer << std::endl;
			}
		}
		return logined;
	}

	std::string UserDetails::getRandomPassword(int size /*= 12*/) const
	{
		std::string password;
		int s0 = rand() % size;
		int s1;
		do
		{
			s1 = rand() % size;
		} while (s1 == s0);
		for (int i = 0; i < size; i++)
		{
			if (i == s0 || i == s1)
			{
				char s[] = "-+._=@!?,;~#()[]{}*/";
				password += s[rand() % (sizeof(s) - 1)];
			}
			else
			{
				int t = rand() % 12;
				if (t < 4)
				{
					password += (char)('a' + (rand() % ('z' - 'a' + 1)));
				}
				else if (t < 8)
				{
					password += (char)('A' + (rand() % ('Z' - 'A' + 1)));
				}
				else
				{
					password += (char)('0' + (rand() % ('9' - '0' + 1)));
				}
			}
		}
		return password;
	}

	bool UserDetails::sendFile(const std::string& username, const std::string& password, const std::string& path, nlohmann::json& modInfo, const std::string& file)
	{
		if (modInfo.count("name") == 0)
		{
			std::cout << "Error : mod name cannot be empty\n";
			return false;
		}
		std::string invalidChars = getInvalidCharsInUsername(modInfo["name"].get<std::string>());
		if (invalidChars.size() > 0)
		{
			std::cout << "Error : mod name has invalid characters:" << invalidChars << "\n";
			return false;
		}
		if (modInfo.count("icon"))
		{
			DirentHelper::File f;
			f.file = modInfo["icon"].get<std::string>();
			if (f.isTypeOf("png") == false)
			{
				std::cout << "Error : mod icon must be a png file\n";
				return false;
			}
		}

		std::string last_folder_path = path;
		fixFilePath(last_folder_path);
		if (last_folder_path[last_folder_path.size() - 1] == '/')
		{
			last_folder_path.resize(last_folder_path.size() - 1);
		}

		std::size_t found = last_folder_path.find_last_of('/');
		if (found != std::string::npos)
		{
			last_folder_path = last_folder_path.substr(found + 1);
		}

		if (last_folder_path.size() == 0)
		{
			std::cout << "Error : folder name error\n";
			return false;
		}
		bool success = false;
		std::string readBuffer = "";
		CURL *curl = curl_easy_init();
		if (curl)
		{
			struct curl_httppost *formpost = NULL;
			struct curl_httppost *lastptr = NULL;
			struct curl_slist *headerlist = NULL;

			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "foldername",
				CURLFORM_COPYCONTENTS, last_folder_path.c_str(),
				CURLFORM_END);

			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "name",
				CURLFORM_COPYCONTENTS, modInfo["name"].get<std::string>().c_str(),
				CURLFORM_END);

			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "name",
				CURLFORM_COPYCONTENTS, modInfo["name"].get<std::string>().c_str(),
				CURLFORM_END);

			if (modInfo.count("description"))
			{
				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "description",
					CURLFORM_COPYCONTENTS, modInfo["description"].get<std::string>().c_str(),
					CURLFORM_END);
			}
			if (modInfo.count("version"))
			{
				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "version",
					CURLFORM_COPYCONTENTS, modInfo["version"].get<std::string>().c_str(),
					CURLFORM_END);
			}
			if (modInfo.count("creator"))
			{
				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "creator",
					CURLFORM_COPYCONTENTS, modInfo["creator"].get<std::string>().c_str(),
					CURLFORM_END);
			}

			if (modInfo.count("icon"))
			{
				curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "icon",
					CURLFORM_FILE, (path + modInfo["icon"].get<std::string>()).c_str(),
					CURLFORM_END);
			}

			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "username",
				CURLFORM_COPYCONTENTS, username.c_str(),
				CURLFORM_END);

			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "password",
				CURLFORM_COPYCONTENTS, password.c_str(),
				CURLFORM_END);

			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "userfile",
				CURLFORM_FILE, file.c_str(),
				CURLFORM_END);

			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "upload",
				CURLFORM_COPYCONTENTS, "upload",
				CURLFORM_END);

			curl_easy_setopt(curl, CURLOPT_URL, "http://bloodworks.enginmercan.com/upload.php");
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

			CURLcode res = curl_easy_perform(curl);
			if (res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

			success = readBuffer == "true";
			curl_easy_cleanup(curl);

			curl_formfree(formpost);
			curl_slist_free_all(headerlist);
		}

		if (success)
		{
			std::cout << "Success!\n";
		}
		else
		{
			std::cout << "Error :" << readBuffer << "\n";
		}
		return success;
	}

	bool isLoginned() const
	{
		return logined;
	}
	bool hasStoredUser() const
	{
		return username.size() > 0 && password.size() > 0;
	}

	const std::string& getUsername() const
	{
		return username;
	}

	const std::string& getPassword() const
	{
		return password;
	}
};