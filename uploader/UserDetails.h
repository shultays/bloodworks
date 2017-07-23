#pragma once

#include <string>

class UserDetails
{
	std::string username;
	std::string password;

	bool valids[256];
	bool logined;
public:
	UserDetails();

	const std::string getInvalidCharsInUsername(const std::string& username) const;
	const std::string getInvalidCharsInPassword(const std::string& password) const;

	bool isLoginned() const
	{
		return logined;
	}
	bool hasStoredUser() const
	{
		return username.size() > 0 && password.size() > 0;
	}

	bool tryLogin(const std::string& username, const std::string& password);
	bool tryLogin();
	bool tryRegister(const std::string& username, const std::string& password);
	std::string getRandomPassword(int size = 12) const;

	bool sendFile(const std::string& username, const std::string& password, const std::string& file);
	const std::string& getUsername() const
	{
		return username;
	}

	const std::string& getPassword() const
	{
		return password;
	}
};