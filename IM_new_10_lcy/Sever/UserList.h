#ifndef __USERLIST_H__
#define __USERLIST_H__

#include <winsock2.h>
#include "../Client/FriList.h"

struct USER 
{
	char m_szID[11];
	char m_szPsd[13];
	char m_szName[20];
	char m_szSex[4];
	char m_szAge[4];
	char m_szEmail[20];
	char m_szState[6];
	SOCKET m_sSocket;
	FRIEND *m_pFriHead;
	FRIEND *m_pFriEnd;

	USER *m_pPre;
	USER *m_pNext;
};

int AddNode(USER *pNode);
USER *FindUser(const char *pID);
int LoadDate();
int SaveDate();

extern USER *g_pUserHead;
extern USER *g_pUserEnd;

#endif