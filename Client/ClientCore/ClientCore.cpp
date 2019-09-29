#include "./ClientCore.hpp"
#include "./../ReturningCodes.h"
#include "./../../hashlib2plus/hashlibpp.h"

#include <process.h>

errno_t ClientCore::StartListenForMsg()
{
	errno_t err;

	if (Connected && !ListenOn)
	{
		if ((hListenThread = _beginthreadex(NULL, 0, &this->NewThreadListenForMsg, static_cast<void*>(this), CREATE_SUSPENDED, &iListenThread)))
		{
			err = RTN_SUCCESS;
		}
		else
		{
			err = CLIENT_THREAD;
		}
	}
	else
	{
		err = CLIENT_DISCONNECTED;
	}

	return (err);
}

unsigned __stdcall ClientCore::NewThreadListenForMsg(void* ptrClass)
{
	ClientCore* ptr = static_cast<ClientCore*>(ptrClass);
	ptr->ListenOn = true;
	ptr->WaitForMsg();
	ptr->ListenOn = false;
	return (RTN_SUCCESS);
}

void ClientCore::WaitForMsg()
{
	int szBuf = 0;
	while ((szBuf = recv(Sock, &Buff[0], 1023, 0)) != SOCKET_ERROR && szBuf != 0)
	{
		Buff[szBuf] = '\0';
		InputMsg(&Buff[0]);
	}

	if (Connected)
	{
		Disconnect();
		std::string str = "\nLost connection to the server: " + static_cast<std::string>(&ServerName[0]) + "\n\n" + "root@Client: ";
		InputMsg(&str[0]);
	}

	return;
}

ClientCore::ClientCore()
{
	WSAStartup(0x0202, &WSADat);
	gethostname(&PCName[0], 30);
	hashwrapper* h = new md5wrapper();
	HashClientToken = h->getHashFromFile("./tokens/ClientToken");
	HashServerToken = h->getHashFromFile("./tokens/ServerToken");
	delete h;
}

ClientCore::~ClientCore()
{
	closesocket(Sock);
	WSACleanup();
}

char* ClientCore::GetPCName()
{
	return (&PCName[0]);
}

char* ClientCore::GetServerName()
{
	return (&ServerName[0]);
}

uintptr_t ClientCore::GetHandleListenThread()
{
	return (hListenThread);
}

unsigned ClientCore::GetIdListenThread()
{
	return (iListenThread);
}

bool ClientCore::isConnected()
{
	return (Connected);
}

errno_t ClientCore::SendMsg(char* msg)
{
	errno_t err;

	if (Connected) 
	{
		if (send(Sock, &msg[0], static_cast<int>(strlen(&msg[0])), 0) != SOCKET_ERROR)
		{
			err = RTN_SUCCESS;
		}
		else
		{
			err = CLIENT_SEND_MSG;
		}
	}
	else
	{
		err = CLIENT_DISCONNECTED;
	}

	return (err);
}

errno_t ClientCore::Connect(char* ip, u_short port, char* login, char* password)
{
	errno_t err;

	if (Connected)
	{
		err = CLIENT_CONNECTED;
		return (err);
	}

	DestAddr.sin_family = AF_INET;
	DestAddr.sin_addr.s_addr = inet_addr(&ip[0]);
	DestAddr.sin_port = htons(port);

	Sock = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(Sock, reinterpret_cast<sockaddr*>(&DestAddr), sizeof(DestAddr)) == SOCKET_ERROR)
	{
		closesocket(Sock);
		err = CLIENT_CONNECT;
		return (err);
	}

	send(Sock, &HashClientToken[0], 33, 0);
	recv(Sock, &Buff[0], 33, 0);
	if (strcmp(&Buff[0], &HashServerToken[0]))
	{
		closesocket(Sock);
		err = CLIENT_TOKEN;
		return (err);
	}

	send(Sock, &PCName[0], static_cast<int>(strlen(&PCName[0])), 0);
	recv(Sock, &ServerName[0], 30, 0);

	send(Sock, &login[0], static_cast<int>(strlen(&login[0])), 0);
	hashwrapper* h = new md5wrapper();
	std::string md5 = h->getHashFromString(&password[0]);
	Sleep(50);
	send(Sock, &md5[0], static_cast<int>(md5.size()), 0);
	delete h;
	md5.clear();

	Connected = true;
	StartListenForMsg();
	err = RTN_SUCCESS;

	return (err);
}

errno_t ClientCore::Disconnect()
{
	errno_t err;

	if (!Connected)
	{
		err = CLIENT_DISCONNECTED;
	}
	else
	{
		Connected = false;
		closesocket(Sock);
		err = RTN_SUCCESS;
	}

	return (err);
}
