#ifndef __THREADFUNC_H__
#define __THREADFUNC_H__

#include "../Client/Protocol.h"
#include "DataType.h"


struct PRO_MAP
{
	int m_nProtocolID;
	int (*m_pProtofun)(DATA_PACK *pData);
};

DWORD WINAPI RecvThread(LPVOID lpLparam);
DWORD WINAPI CaclThread(LPVOID lpLparam);

int RegApply(DATA_PACK *pData);
//int RegAsw(DATA_PACK *pData);
int LoginApply(DATA_PACK *pData);
//int LoginAsw(DATA_PACK *pData);
int AddApply(DATA_PACK *pData);
int AddAsw(DATA_PACK *pData);
//int ChatFriend(DATA_PACK *pData);
int DeleteApply(DATA_PACK *pData);
//int DeleteAsw(DATA_PACK *pData);
int RefreshApply(DATA_PACK *pData);
//int RefreshAsw(DATA_PACK *pData);
int UserInfo(DATA_PACK *pData);
int MessageSend(DATA_PACK *pData);
int MessageRecv(DATA_PACK *pData);


extern PRO_MAP g_ProMap[];

#endif