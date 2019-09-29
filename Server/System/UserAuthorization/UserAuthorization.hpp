#ifndef _USER_AUTHORIZATION_H
#define _USER_AUTHORIZATION_H

#include <vector>
#include <string>

class UserAuthorization
{
	private:
		struct UserInfo {
			std::string login = "";
			std::string hashPass = "";
			unsigned char rights = 0;
			bool online = false;
		};

		std::vector<UserInfo> UsersList;

	protected:
		UserAuthorization();
		~UserAuthorization();
		std::vector<std::string> GetUsersList();
		errno_t LoadUsersList();
		errno_t SaveUsersList();
		errno_t AddUser(char* login, char* hashPass, unsigned char rights);
		errno_t SubUser(char* login);
		errno_t Authorization(char* login, char* hashPass);
		errno_t DeAuth(char* login);
};

#endif //_USER_AUTHORIZATION_H
