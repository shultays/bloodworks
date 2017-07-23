
#pragma warning(disable: 4996)

#include "../game/DirentHelper.h"
#include "../game/cPackHelper.h"
#include "../game/UserDetails.h"

#include <iostream>

int main(int argn, const char* argv[])
{
	srand((unsigned)time(0));
	curl_global_init(CURL_GLOBAL_DEFAULT);
	UserDetails account;
	if (account.hasStoredUser())
	{
		account.tryLogin();
	}

	while (account.isLoginned() == false)
	{
		std::string username, password;
		int answer;
		do
		{
			std::cout << "1 - Create new user\n2 - Login with existing user\n";
			std::cin >> answer;
		} while (answer != 1 && answer != 2);

		if (answer == 1 || answer == 2)
		{
			if (answer == 1)
			{
				std::cout << "Creating new user...\n";
			}
			else if (answer == 2)
			{
				std::cout << "Logining with existing user...\n";
			}
			while (true)
			{
				username = "";
				password = "";
				std::cout << "Enter username:\n";

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				getline(std::cin, username);

				username.erase(std::remove(username.begin(), username.end(), '\n'), username.end());
				username.erase(std::remove(username.begin(), username.end(), '\r'), username.end());

				if (username == "")
				{
					break;
				}

				std::string invalidChars = account.getInvalidCharsInUsername(username);
				if (invalidChars.size() > 0)
				{
					std::cout << "Invalid characters in username : " << invalidChars << "\n";
					continue;
				}
				if (answer == 1)
				{
					std::cout << "Enter password (leave empty for a random password):\n";
				}
				else if (answer == 2)
				{
					std::cout << "Enter password:\n";
				}
				getline(std::cin, password);

				password.erase(std::remove(password.begin(), password.end(), '\n'), password.end());
				password.erase(std::remove(password.begin(), password.end(), '\r'), password.end());

				invalidChars = account.getInvalidCharsInPassword(password);
				if (invalidChars.size() > 0)
				{
					std::cout << "Invalid characters in password : " << invalidChars << "\n";
					continue;
				}

				if (password == "")
				{
					if (answer == 1)
					{
						password = account.getRandomPassword(12);
						std::cout << "Random password : " << password;
						std::cout << "\nNote it and press enter.\n";
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
					else
					{
						break;
					}
				}

				if (answer == 1)
				{
					account.tryRegister(username, password);
				}
				else if (answer == 2)
				{
					account.tryLogin(username, password);
				}

				break;
			}
		}
	}

	std::string path;
	
	if (argn > 1)
	{
		path = argv[1];
	}
	if (path.length() == 0)
	{
		std::cout << "Enter a mod folder to upload:\n";
		std::cin >> path;
	}


	std::string tempName = "uploader/temp.bld";
	bool done = cPackHelper::packFolder(path, tempName);
	if (done)
	{
		account.sendFile(account.getUsername(), account.getPassword(), tempName);
		cPackHelper::deleteFile(tempName);
	}
	curl_global_cleanup();

#ifdef _DEBUG
	std::cin >> path;
#endif
    return 0;
}

