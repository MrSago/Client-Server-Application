#ifndef _SYSTEM_CORE_H
#define _SYSTEM_CORE_H

#include "./../../ServerCore/ServerCore.hpp"
#include "./../UserAuthorization/UserAuthorization.hpp"

class SystemCore : private UserAuthorization, private ServerCore
{
	private:
		struct CmdList_t {
			std::string cmd = "";
			errno_t (SystemCore::* function)(std::vector<std::string>&) = NULL;
			unsigned char rights = 0;
		};

		std::vector<CmdList_t> CmdList = {
			{ "$help",			&SystemCore::PrintHelp },
			{ "$user_print",	&SystemCore::PrintUsers },
			{ "$user_create",	&SystemCore::CreateUser },
			{ "$user_delete",	&SystemCore::DeleteUser },
			{ "$shutdown",		&SystemCore::ShutDown }
		};

		errno_t ParseCmd(std::vector<std::string> args);
		/*=============System Commands=============*/
		errno_t PrintHelp(std::vector<std::string>&);
		errno_t PrintUsers(std::vector<std::string>&);
		errno_t CreateUser(std::vector<std::string>&);
		errno_t DeleteUser(std::vector<std::string>&);
		errno_t ShutDown(std::vector<std::string>&);
		/*=========================================*/

		errno_t AdminInterface();
		void ClientDoSomething(SOCKET);

	public:
		SystemCore();
		~SystemCore();
		errno_t Start(u_short port);

};

#endif //_SYSTEM_CORE_H
