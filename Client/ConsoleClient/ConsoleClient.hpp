#ifndef _CONSOLE_CLIENT_H
#define _CONSOLE_CLIENT_H

#include "./../ClientCore/ClientCore.hpp"
#include "./../ReturningCodes.h"

#include <vector>

class ConsoleClient : private ClientCore
{
	private:
		struct CmdList_t {
			std::string cmd = "";
			errno_t (ConsoleClient::* function)(std::vector<std::string>&) = NULL;
		};

		std::vector<CmdList_t> CmdList = {
			{ "!help",			&ConsoleClient::PrintHelp },
			{ "!connect",		&ConsoleClient::Connect },
			{ "!disconnect",	&ConsoleClient::Disconnect },
			{ "!quit",			&ConsoleClient::Quit }
		};

		errno_t ParseCmd(std::vector<std::string> args);
		/*=============Client Commands=============*/
		errno_t PrintHelp(std::vector<std::string>&);
		errno_t Connect(std::vector<std::string>&);
		errno_t Disconnect(std::vector<std::string>&);
		errno_t Quit(std::vector<std::string>&);
		/*=========================================*/

		void InputMsg(const char* msg);

	public:
		ConsoleClient();
		~ConsoleClient();
		errno_t Start();
};

#endif //_CONSOLE_CLIENT_H
