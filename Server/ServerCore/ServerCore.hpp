#ifndef _SERVER_CORE_H
#define _SERVER_CORE_H

#include <windows.h>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "mswsock.lib")
#include <string>

class ServerCore
{
private:
	char HostName[30] = { "none" };
	WSAData WSADat;
	sockaddr_in HostAddr;
	u_short HostPort;
	SOCKET HostSock;
	SOCKET CurClientSock = 0;
	size_t nClients = 0;
	uintptr_t hWaitThread;
	unsigned iWaitThread;
	std::string HashClientToken;
	std::string HashServerToken;

	errno_t StartWaitForClients();
	static unsigned __stdcall NewThreadWaitForClients(void* ptrClass);
	void WaitForClients();

	errno_t StartClientThread();
	static unsigned __stdcall NewThreadClientDoSomething(void* ptrClass);

protected:
	ServerCore();
	~ServerCore();
	errno_t Start(u_short port);
	char* GetHostName();

	virtual void ClientDoSomething(SOCKET) = 0;
};

#endif //_SERVER_CORE_H
