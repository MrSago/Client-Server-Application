#include "./UserAuthorization.hpp"
#include "./../../ReturningCodes.h"
#include "./../../../hashlib2plus/hashlibpp.h"

UserAuthorization::UserAuthorization()
{
	LoadUsersList();
}

UserAuthorization::~UserAuthorization()
{
	SaveUsersList();
	UsersList.clear();
}

std::vector<std::string> UserAuthorization::GetUsersList()
{
	std::vector<std::string> list(UsersList.size() + 1);

	for (size_t i = 0; i < UsersList.size(); ++i)
	{
		char _rights[5];
		_itoa_s(static_cast<int>(UsersList[i].rights), &_rights[0], 5, 10);

		std::string _online;
		if (UsersList[i].online)
		{
			_online = "true";
		}
		else
		{
			_online = "false";
		}

		std::string tmp =
			"Login: " + UsersList[i].login + '\n' +
			"Hash Password: " + UsersList[i].hashPass + '\n' +
			"Rights: " + _rights + '\n' +
			"Online: " + _online + '\n';

		list.push_back(tmp);
	}

	return (list);
}

errno_t UserAuthorization::LoadUsersList()
{
	errno_t err;
	FILE* file;
	if (fopen_s(&file, "users.bin", "rb") != 0 || file == static_cast<FILE*>(NULL))
	{
		err = AUTH_FILE;
		return (err);
	}

	size_t sz;
	fread_s(&sz, sizeof(size_t), sizeof(size_t), 1, file);

	for (size_t i = 0; i < sz; ++i)
	{
		UserInfo tmp;
		unsigned char szStr;
		char login[256];
		char pass[256];

		//Login
		fread_s(&szStr, sizeof(unsigned char), sizeof(unsigned char), 1, file);
		fread_s(&login[0], 256, sizeof(char), szStr, file);
		login[szStr] = '\0';
		tmp.login = login;

		//Hash Password
		fread_s(&szStr, sizeof(unsigned char), sizeof(unsigned char), 1, file);
		fread_s(&pass[0], 256, sizeof(char), szStr, file);
		pass[szStr] = '\0';
		tmp.hashPass = pass;

		//Rights
		fread_s(&tmp.rights, sizeof(unsigned char), sizeof(unsigned char), 1, file);

		UsersList.push_back(tmp);
	}

	fclose(file);
	err = RTN_SUCCESS;

	return (err);
}

errno_t UserAuthorization::SaveUsersList()
{
	errno_t err;
	FILE* file;
	if (fopen_s(&file, "users.bin", "wb") != 0 || file == static_cast<FILE*>(NULL))
	{
		err = AUTH_FILE;
		return (err);
	}

	size_t sz = UsersList.size();
	fwrite(&sz, sizeof(size_t), 1, file);

	for (size_t i = 0; i < sz; ++i)
	{
		unsigned char szStr;

		//Login
		szStr = (unsigned char)UsersList[i].login.size();
		fwrite(&szStr, sizeof(unsigned char), 1, file);
		fwrite(&UsersList[i].login[0], sizeof(char), szStr, file);

		//Hash Password
		szStr = (unsigned char)UsersList[i].hashPass.size();
		fwrite(&szStr, sizeof(unsigned char), 1, file);
		fwrite(&UsersList[i].hashPass[0], sizeof(char), szStr, file);
		
		//Rights
		fwrite(&UsersList[i].rights, sizeof(unsigned char), 1, file);
	}

	fclose(file);
	err = RTN_SUCCESS;

	return (err);
}

errno_t UserAuthorization::AddUser(char* login, char* pass, unsigned char rights)
{
	errno_t err;
	UserInfo user;

	//Login
	user.login = &login[0];

	//Hash Password
	hashwrapper* h = new md5wrapper();
	user.hashPass = h->getHashFromString(&pass[0]);
	delete h;

	//Rights
	user.rights = rights;

	UsersList.push_back(user);
	err = SaveUsersList();

	return (err);
}
errno_t UserAuthorization::SubUser(char* login)
{
	errno_t err = AUTH_SUB;

	for (size_t i = 0; i < UsersList.size(); ++i)
	{
		if (UsersList[i].login == &login[0])
		{
			UsersList.erase(UsersList.begin() + i);
			err = RTN_SUCCESS;
			return (err);
		}
	}

	return (err);
}

errno_t UserAuthorization::Authorization(char* login, char* hashPass)
{
	errno_t err = AUTH_FAIL;

	for (size_t i = 0; i < UsersList.size(); ++i)
	{
		if (UsersList[i].login == login && UsersList[i].hashPass == hashPass)
		{
			if (!UsersList[i].online)
			{
				UsersList[i].online = true;
				err = RTN_SUCCESS;
				return (err);
			}
			else
			{
				err = AUTH_ONLINE;
				return (err);
			}
		}
	}

	return (err);
}

errno_t UserAuthorization::DeAuth(char* login)
{
	errno_t err = DEAUTH_FAIL;

	for (size_t i = 0; i < UsersList.size(); ++i)
	{
		if (UsersList[i].login == login)
		{
			UsersList[i].online = false;
			err = RTN_SUCCESS;
			return (err);
		}
	}

	return (err);
}
