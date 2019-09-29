#include "./ConsoleClient.hpp"

#include <iostream>
#include <sstream>

errno_t ConsoleClient::ParseCmd(std::vector<std::string> args)
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
-----------------Client Commands-----------------
-----------------------------------------------*/
errno_t ConsoleClient::PrintHelp(std::vector<std::string>&)
{
	std::cout << "List of client commands:" << std::endl;
	std::cout << "!help - print this help" << std::endl;
	std::cout << "!connect [IP:PORT] [Login] [Password] - connect to the server" << std::endl;
	std::cout << "!disconnect - disconnect from the server" << std::endl;
	std::cout << "!quit - Exit from programm\n" << std::endl;
	return (CMD_PRINTHELP);
}

errno_t ConsoleClient::Connect(std::vector<std::string>& args)
{
	int tmp;
	errno_t err;
	char* ip;
	u_short port;
	unsigned char iAddress = 0, iLogin = 1, iPassword = 2;

	if (args.size() != 3)
	{
		err = CMD_ARGS;
		return (err);
	}

	sscanf_s(&args[iAddress][0], "%d.%d.%d.%d:%hu", &tmp, &tmp, &tmp, &tmp, &port);
	ip = strtok(static_cast<char*>(&args[iAddress][0]), ":");

	std::cout << "Connecting...";
	if (!(err = ClientCore::Connect(&ip[0], port, &args[iLogin][0], &args[iPassword][0])))
	{
		err = CMD_CONNECT;
	}
	std::cout << std::endl;

	return (err);
}

errno_t ConsoleClient::Disconnect(std::vector<std::string>&)
{
	errno_t err;
	if (!(err = ClientCore::Disconnect()))
	{
		err = CMD_DISCONNECT;
	}
	return (err);
}

errno_t ConsoleClient::Quit(std::vector<std::string>&)
{
	return (CMD_QUIT);
}
/*-------------------------*/

void ConsoleClient::InputMsg(const char* msg)
{
	std::cout << &msg[0];
}

ConsoleClient::ConsoleClient() {}

ConsoleClient::~ConsoleClient() {}

errno_t ConsoleClient::Start()
{
	errno_t err = CMD_UNKNOWN;

	std::cout << "Client v0.0.7 started!" << std::endl;
	std::cout << "Type !help for command list\n" << std::endl;

	while (err != CMD_QUIT)
	{
		std::vector<std::string> msg;

		std::string str;
		while (!str.size())
		{
			if (!ClientCore::isConnected())
			{
				std::cout << "root@Client: ";
			}

			std::getline(std::cin, str);
		}

		std::istringstream ist(str);
		std::string tmp;
		while (ist >> tmp)
		{
			msg.push_back(tmp);
		}

		if (msg[0][0] == '!')
		{
			err = ParseCmd(msg);
			switch (err)
			{
			case CLIENT_CONNECTED:
				std::cout << "You already connected!" << std::endl << std::endl;
				break;

			case CLIENT_DISCONNECTED:
				std::cout << "You are not connected to the server!" << std::endl << std::endl;
				break;

			case CLIENT_CONNECT:
				std::cout << "Error connecting to the server!" << std::endl << std::endl;
				break;

			case CLIENT_TOKEN:
				std::cout << "Incorrect tokens!" << std::endl << std::endl;
				break;

			case CMD_UNKNOWN:
				std::cout << "Unknown command: " << &msg[0][0] << std::endl << std::endl;
				break;

			case CMD_ARGS:
				std::cout << "Invalid arguments!" << std::endl << std::endl;
				break;

			case CMD_PRINTHELP:
				break;

			case CMD_CONNECT:
				std::cout << "Connected to " << &(ClientCore::GetServerName())[0] << std::endl << std::endl;
				ResumeThread(reinterpret_cast<HANDLE>(ClientCore::GetHandleListenThread()));
				break;

			case CMD_DISCONNECT:
				std::cout << "Disconnected!" << std::endl << std::endl;
				break;

			case CMD_QUIT:
				ClientCore::Disconnect();
				std::cout << "Exit from programm...";
				Sleep(1000);
				break;
			}
		}
		else
		{
			std::string tmp = msg[0];
			for (size_t i = 1; i < msg.size(); ++i)
			{
				tmp += ' ' + msg[i];
			}

			switch (ClientCore::SendMsg(&tmp[0]))
			{
			case CLIENT_SEND_MSG:
				std::cout << "Error sending message!" << std::endl << std::endl;
				break;

			case CLIENT_DISCONNECTED:
				std::cout << "You are not connected to the server!" << std::endl << std::endl;
				break;

			case RTN_SUCCESS:
				break;
			}
		}
	}

	return (err);
}
