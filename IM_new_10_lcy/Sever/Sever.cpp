// Sever.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Client/Protocol.h"

#include "UserList.h"
#include "DataType.h"
#include "ThreadFunc.h"

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI RecvProc(LPVOID lpParameter);

int main(int argc, char* argv[])
{
	WSADATA wd;
	
	WSAStartup(0x0202, &wd);
	CLIENT_INFO slisten;
	
	slisten.m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	slisten.m_addrClient.sin_family = AF_INET;
	slisten.m_addrClient.sin_port = htons(6060);
	slisten.m_addrClient.sin_addr.s_addr = inet_addr("192.168.2.136");
	
	if(INVALID_SOCKET == slisten.m_sClient)
	{
		return -1;
	}
	
	if(SOCKET_ERROR == bind(slisten.m_sClient, (sockaddr *)&(slisten.m_addrClient), sizeof(sockaddr)))
	{
		closesocket(slisten.m_sClient);
		
		return -1;
	}
	
	if(SOCKET_ERROR == listen(slisten.m_sClient, 5))
	{
		closesocket(slisten.m_sClient);
		
		return -1;
	}
	
	int nSize = sizeof(sockaddr);
	CLIENT_INFO *pClient = NULL;
	LoadDate();
	
	while (TRUE)
	{
		pClient = new CLIENT_INFO;
		
		pClient->m_sClient = accept(slisten.m_sClient, (sockaddr *)&(pClient->m_addrClient), &nSize);
		
		if(INVALID_SOCKET == pClient->m_sClient)
		{
			delete pClient;
			
			return -1;
		}
		
		USHORT usPort = pClient->m_addrClient.sin_port;

		cout<<inet_ntoa(pClient->m_addrClient.sin_addr)<<"*****"<<usPort<<" :Login!"<<endl;
		CloseHandle(CreateThread(NULL, 0, RecvThread, (LPVOID)pClient, 0, 0));
	}
	
	closesocket(slisten.m_sClient);
	
	WSACleanup();

	return 0;
}
