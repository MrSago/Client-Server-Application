#include "./SystemCore.hpp"
#include "./../../ReturningCodes.h"

#include <iostream>
#include <sstream>

errno_t SystemCore::AdminInterface()
{
	errno_t err = CMD_UNKNOWN;

	while (err != CMD_SHUTDOWN)
	{
		std::vector<std::string> msg;

		std::string str;
		while (!str.size())
		{
			std::cout << "root@" << &ServerCore::GetHostName()[0] << ": ";
			std::getline(std::cin, str);
		}

		std::istringstream ist(str);
		std::string tmp;
		while (ist >> tmp)
		{
			msg.push_back(tmp);
		}

		if (msg[0][0] == '$')
		{
			err = ParseCmd(msg);
			switch (err)
			{
			case AUTH_FILE:
				std::cout << "Error saving users.bin file!" << std::endl;
				break;

			case AUTH_SUB:
				std::cout << "Can't find this user!" << std::endl;
				break;

			case CMD_UNKNOWN:
				std::cout << "Unknown command: " << &msg[0][0] << std::endl << std::endl;
				break;

			case CMD_ARGS:
				std::cout << "Invalid arguments!" << std::endl << std::endl;
				break;

			case CMD_PRINTHELP:
				break;

			case CMD_PRINTUSERS:
				break;

			case CMD_CREATEUSER:
				std::cout << "User created!" << std::endl << std::endl;
				break;

			case CMD_DELETEUSER:
				std::cout << "User deleted!" << std::endl << std::endl;
				break;

			case CMD_SHUTDOWN:
				std::cout << "Shutting down server!" << std::endl << std::endl;
				break;
			}
		}
	}

	return (err);
}

void SystemCore::ClientDoSomething(SOCKET clientSock)
{
	char buff[1024];
	int bytesRecv;
	char login[256], hashPass[256];

	const char* strAuth = { "Authorization on server...\n" };
	send(clientSock, &strAuth[0], static_cast<int>(strlen(&strAuth[0])), 0);
	bytesRecv = recv(clientSock, &login[0], 255, 0);
	login[bytesRecv] = '\0';
	bytesRecv = recv(clientSock, &hashPass[0], 255, 0);
	hashPass[bytesRecv] = '\0';

	if (!UserAuthorization::Authorization(&login[0], &hashPass[0]))
	{
		const char* strSuc = { "Success!\n" };
		send(clientSock, &strSuc[0], static_cast<int>(strlen(&strSuc[0])), 0);
	}
	else
	{
		const char* strFail = { "Failure!\n" };
		send(clientSock, &strFail[0], static_cast<int>(strlen(&strFail[0])), 0);
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	std::string userStr = "\n";
	userStr += &ServerCore::GetHostName()[0];
	userStr += '@';
	userStr += &login[0];
	userStr += ": ";
	send(clientSock, &userStr[0], static_cast<int>(userStr.size()), 0);
	while ((bytesRecv = recv(clientSock, &buff[0], sizeof(buff), 0)) && bytesRecv != SOCKET_ERROR)
	{
		send(clientSock, &buff[0], bytesRecv, 0);
		send(clientSock, &userStr[0], static_cast<int>(userStr.size()), 0);
	}

	UserAuthorization::DeAuth(&login[0]);

	return;
}

errno_t SystemCore::ParseCmd(std::vector<std::string> args)
{
	errno_t err = CMD_UNKNOWN;

	for (size_t i = 0; i < CmdList.size(); ++i)
	{
		if (CmdList[i].cmd == args[0])
		{
			args.erase(args.begin());
			err = (this->*CmdList[i].function)(args);
			return (err);
		}
	}

	return (err);
}

/*-----------------------------------------------
-----------------Server Commands-----------------
-------------------------------------------------*/
errno_t SystemCore::PrintHelp(std::vector<std::string>&)
{
	std::cout << "List of server commands:" << std::endl;
	std::cout << "$help - print this help" << std::endl;
	std::cout << "$user_print - print registered users" << std::endl;
	std::cout << "$user_create [login] [password] [rights] - create new user" << std::endl;
	std::cout << "$user_delete [login] - delete user" << std::endl;
	std::cout << "$shutdown - shutting down server\n" << std::endl;
	return (CMD_PRINTHELP);
}

errno_t SystemCore::PrintUsers(std::vector<std::string>&)
{
	std::vector<std::string> list = UserAuthorization::GetUsersList();

	std::cout << "Users List: ";
	if (list.size())
	{
		std::cout << std::endl;
		for (size_t i = 0; i < list.size(); ++i)
		{
			std::cout << list[i] << std::endl;
		}
	}
	else
	{
		std::cout << "[list empty]" << std::endl;
	}
	std::cout << std::endl;

	return (CMD_PRINTUSERS);
}

errno_t SystemCore::CreateUser(std::vector<std::string>& args)
{
	errno_t err;

	if (args.size() == 3)
	{
		if (!(err = UserAuthorization::AddUser(&args[0][0], &args[1][0], static_cast<unsigned char>(atoi(&args[2][0])))))
		{
			err = CMD_CREATEUSER;
		}
	}
	else
	{
		err = CMD_ARGS;
	}

	return (err);
}

errno_t SystemCore::DeleteUser(std::vector<std::string>& args)
{
	errno_t err;

	if (args.size() == 1)
	{
		if (!(err = UserAuthorization::SubUser(&args[0][0])))
		{
			err = CMD_DELETEUSER;
			return (err);
		}
	}
	else
	{
		err = CMD_ARGS;
	}

	return (err);
}

errno_t SystemCore::ShutDown(std::vector<std::string>&)
{
	return (CMD_SHUTDOWN);
}
/*-------------------------*/

SystemCore::SystemCore() {}

SystemCore::~SystemCore() {}

errno_t SystemCore::Start(u_short port)
{
	ServerCore::Start(port);

	std::cout << "Starting server v1.3.3.7..." << std::endl;
	std::cout << "Address: [any ip]" << std::endl;
	std::cout << "Port: " << port << std::endl << std::endl;

	errno_t err = AdminInterface();
	return (err);
}
