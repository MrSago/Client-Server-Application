#ifndef _CLIENT_CORE_H
#define _CLIENT_CORE_H

#include <windows.h>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "mswsock.lib")
#include <string>

class ClientCore
{
	private:
		char PCName[30] = { "none" };
		char ServerName[30] = { "none" };
		char Buff[1024];
		WSAData WSADat;
		sockaddr_in DestAddr;
		SOCKET Sock;
		uintptr_t hListenThread;
		unsigned iListenThread;
		std::string HashClientToken;
		std::string HashServerToken;
		bool Connected = false;
		bool ListenOn = false;

		errno_t StartListenForMsg();
		static unsigned __stdcall NewThreadListenForMsg(void* ptrClass);
		void WaitForMsg();

	protected:
		ClientCore();
		~ClientCore();
		char* GetPCName();
		char* GetServerName();
		uintptr_t GetHandleListenThread();
		unsigned GetIdListenThread();
		bool isConnected();
		errno_t SendMsg(char* msg);
		errno_t Connect(char* ip, u_short port, char* login, char* password);
		errno_t Disconnect();

	private:	virtual void InputMsg(const char* msg) = 0;
	protected:	virtual errno_t Start() = 0;
};

#endif //_CLIENT_CORE_H
