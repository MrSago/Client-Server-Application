#include "./ServerCore.hpp"
#include "./../ReturningCodes.h"
#include "./../../hashlib2plus/hashlibpp.h"

#include <iostream>
#include <process.h>

errno_t ServerCore::StartWaitForClients()
{
	errno_t err;

	if ((hWaitThread = _beginthreadex(NULL, 0, &this->NewThreadWaitForClients, static_cast<void*>(this), 0, &iWaitThread)))
	{
		err = RTN_SUCCESS;
	}
	else
	{
		err = SERVER_THREAD;
	}

	return (err);
}

unsigned __stdcall ServerCore::NewThreadWaitForClients(void* ptrClass)
{
	(static_cast<ServerCore*>(ptrClass))->WaitForClients();
	return (RTN_SUCCESS);
}

void ServerCore::WaitForClients()
{
	char clientName[30] = { "none" };
	SOCKET clientSock;
	sockaddr_in clientAddr;
	int szClientAddr = sizeof(clientAddr);

	while ((clientSock = accept(HostSock, reinterpret_cast<sockaddr*>(&clientAddr), &szClientAddr)))
	{
		CurClientSock = clientSock;
		nClients++;
		StartClientThread();
	}

	return;
}

errno_t ServerCore::StartClientThread()
{
	errno_t err;

	if (_beginthreadex(NULL, 0, &this->NewThreadClientDoSomething, static_cast<void*>(this), 0, NULL))
	{
		err = RTN_SUCCESS;
	}
	else
	{
		err = SERVER_THREAD;
	}

	return (err);
}

unsigned __stdcall ServerCore::NewThreadClientDoSomething(void* ptrClass)
{
	ServerCore* ptr = static_cast<ServerCore*>(ptrClass);
	SOCKET clientSock = ptr->CurClientSock;
	char buff[256];
	char clientName[30];

	recv(clientSock, &buff[0], 33, 0);
	if (strcmp(&buff[0], &ptr->HashClientToken[0]))
	{
		send(clientSock, "null", 5, 0);
		ptr->nClients--;
		closesocket(clientSock);
		return (SERVER_TOKEN);
	}
	send(clientSock, &ptr->HashServerToken[0], 33, 0);

	recv(clientSock, &clientName[0], 30, 0);
	send(clientSock, &ptr->HostName[0], static_cast<int>(strlen(&ptr->HostName[0])), 0);

	std::cout << std::endl << '+' << &clientName[0];
	std::cout << std::endl << "root@" << &ptr->HostName[0] << ": ";

	const char* strWelc = { "Welcome to my server!\n" };
	send(clientSock, &strWelc[0], static_cast<int>(strlen(&strWelc[0])), 0);
	ptr->ClientDoSomething(clientSock);

	std::cout << std::endl << '-' << &clientName[0] << std::endl;
	std::cout << std::endl << "root@" << &ptr->HostName[0] << ": ";

	ptr->nClients--;
	closesocket(clientSock);

	return (RTN_SUCCESS);
}

ServerCore::ServerCore()
{
	WSAStartup(0x0202, &WSADat);
	gethostname(HostName, 30);
	hashwrapper* h = new md5wrapper();
	HashClientToken = h->getHashFromFile("./tokens/ClientToken");
	HashServerToken = h->getHashFromFile("./tokens/ServerToken");
	delete h;
}

ServerCore::~ServerCore()
{
	closesocket(HostSock);
	WSACleanup();
}

errno_t ServerCore::Start(u_short port)
{
	HostPort = port;

	HostAddr.sin_family = AF_INET;
	HostAddr.sin_addr.s_addr = 0;
	HostAddr.sin_port = htons(HostPort);

	HostSock = socket(AF_INET, SOCK_STREAM, 0);

	bind(HostSock, reinterpret_cast<sockaddr*>(&HostAddr), sizeof(HostAddr));

	listen(HostSock, SOMAXCONN);

	StartWaitForClients();

	return (RTN_SUCCESS);
}

char* ServerCore::GetHostName()
{
	return (&HostName[0]);
}
