#include "StdAfx.h"
#include "ThreadFunc.h"
#include "UserList.h"

PRO_MAP g_ProMap[] = {{REG_APPLY, RegApply},
	//					{REG_ASW, RegAsw},
						{LOGIN_APPLY, LoginApply},
			//			{LOGIN_ASW, LoginAsw},
						{ADD_APPLY, AddApply}, 
						{ADD_ASW, AddAsw},
		//				{CHAT_FRIEND, ChatFriend},
						{DELETE_APPLY, DeleteApply}, 
		//				{DELETE_ASW, DeleteAsw}, 
						{REFRESH_APPLY, RefreshApply},
		//				{REFRESH_ASW, RefreshAsw}, 
						{USER_INFO, UserInfo},
						{MESSAGE_RECV, MessageRecv},
						{MESSAGE_SEND, MessageSend},
						{0, NULL}};


DWORD WINAPI RecvThread(LPVOID lpLparam)
{
	CLIENT_INFO *pClient = (CLIENT_INFO *)lpLparam;
	DATA_PACK *pDate = NULL;
	
	while (TRUE)
	{
		pDate = new DATA_PACK;
		pDate->m_pClientIfo = pClient;
		
		if (SOCKET_ERROR == recv(pClient->m_sClient, pDate->m_szBuf, 512, 0))
		{
			USHORT usPort = pClient->m_addrClient.sin_port;

			cout<<inet_ntoa(pClient->m_addrClient.sin_addr)<<"*****"<<usPort<<" :Out!"<<endl;
			
			closesocket(pClient->m_sClient);
			
			delete pClient;
			delete pDate;

			return -1;
		}
		
		CloseHandle(CreateThread(NULL, 0, CaclThread, (LPVOID)pDate, 0, 0));
	}
	


	return 0;
}


DWORD WINAPI CaclThread(LPVOID lpLparam)
{
	DATA_PACK *pData = (DATA_PACK *)lpLparam;

	PRO_MAP *pProMap = g_ProMap;

	if (NULL != pProMap)
	{
		while(NULL != pProMap->m_pProtofun)
		{
			if(*(int *)(pData->m_szBuf) == pProMap->m_nProtocolID)
			{
				pProMap->m_pProtofun(pData);

				break;
			}

			pProMap++;
		}
	}
	

	delete pData;

	return 0;
}

int RegApply(DATA_PACK *pData)
{
	REGIS *pRegis = (REGIS *)(pData->m_szBuf);
	USER *pPos = FindUser(pRegis->m_szID);
	EVNET retEvent;
	
	retEvent.m_nProtoID = REG_ASW;
	
	if(NULL == pPos)
	{
		USER *pNew = new USER;
		strcpy(pNew->m_szState, "离线");
		
		strcpy(pNew->m_szID, pRegis->m_szID);
		strcpy(pNew->m_szPsd, pRegis->m_szPsd);
		strcpy(pNew->m_szName, pRegis->m_szName);
		strcpy(pNew->m_szSex, pRegis->m_szSex);
		strcpy(pNew->m_szAge, pRegis->m_szAge);
		strcpy(pNew->m_szEmail, pRegis->m_szEmail);

		pNew->m_sSocket = pData->m_pClientIfo->m_sClient;
		pNew->m_pNext= NULL;
		pNew->m_pPre = NULL;
		pNew->m_pFriEnd = NULL;
		pNew->m_pFriHead = NULL;
		
		AddNode(pNew);
		
		retEvent.m_nEvent = REG_OK;
		
		FILE *pFile = fopen("AllUserInfo.txt", "a+");
		
		if(NULL == pFile)
		{
			cout<<"Register File!!!";
		}
		
		fwrite(pNew, sizeof(USER), 1, pFile);
		
		fclose(pFile);
		
		FRIEND *pFriend = new FRIEND;
		
		strcpy(pFriend->m_szID, pNew->m_szID);
		strcpy(pFriend->m_szName, pNew->m_szName);
		strcpy(pFriend->m_szState, pNew->m_szState);
		strcpy(pFriend->m_szSex, pNew->m_szSex);
		strcpy(pFriend->m_szState, "离线");
		
		AddFriNode(&(pNew->m_pFriHead), &(pNew->m_pFriEnd), pFriend);
		char szName[30] = "";
		strcpy(szName, pNew->m_szID);
		strcat(szName, "FriList.txt");
		
		FILE *pFile1 = fopen(szName, "a");
		
		fwrite(pFriend, sizeof(FRIEND), 1, pFile1);
		
		fclose(pFile1);
		
		delete pFriend;
	}
	else
	{
		retEvent.m_nEvent = REG_NO;
	}

	send(pData->m_pClientIfo->m_sClient, (char *)&retEvent, sizeof(EVNET), 0);
	
	return 0;
}

int LoginApply(DATA_PACK *pData)
{
	LOGIN *pLogin = (LOGIN *)(pData->m_szBuf);
	USER *pPos = FindUser(pLogin->m_szID);
	EVNET RetEevent;

	RetEevent.m_nProtoID = LOGIN_ASW;

	if(NULL == pPos)
	{
		RetEevent.m_nEvent = LOGIN_NO;
	}
	else
	{
		if (0 == strcmp(pLogin->m_szPsd, pPos->m_szPsd))
		{
			RetEevent.m_nEvent = LOGIN_OK;
		}
		else
		{
			RetEevent.m_nEvent = LOGIN_NO;
		}
	}
	send(pData->m_pClientIfo->m_sClient, (char *)&RetEevent, sizeof(EVNET), 0);

	if(LOGIN_OK == RetEevent.m_nEvent)
	{
		FRIEND *pFri = pPos->m_pFriHead;
	
		strcpy(pFri->m_szState, "在线");
		strcpy(pPos->m_szState, "在线");
		pPos->m_sSocket = pData->m_pClientIfo->m_sClient;

		FRIEND *pFriList = new FRIEND [pPos->m_pFriEnd->m_nCount+1];

		for(int i = 0; NULL != pFri; ++i, pFri = pFri->m_pNext)
		{
			memcpy(&(pFriList[i]), pFri, sizeof(FRIEND));
			pFriList[i].m_ProtoID = CHAT_FRIEND;
			pFriList[i].m_nEvent = pPos->m_pFriEnd->m_nCount+1;
		}


		send(pData->m_pClientIfo->m_sClient, (char *)pFriList, sizeof(FRIEND)*(pPos->m_pFriEnd->m_nCount+1), 0);
		
		delete [sizeof(FRIEND)*(pPos->m_pFriEnd->m_nCount+1)](char *)pFriList;

		USER *pTemp = g_pUserHead;
		
		while (NULL != pTemp)
		{
			FRIEND *pFri = FindNode(pTemp->m_pFriHead, pLogin->m_szID);

			if(NULL != pFri)
			{
				strcpy(pFri->m_szState, "在线");

				if (0 == strcmp(pTemp->m_szState, "在线"))
				{
					STATE RetStates;
					
					RetStates.m_ProtoID = REFRESH_ASW;
					RetStates.m_nEvent = REFRESH_OK;
					strcpy(RetStates.m_szState, "在线");
					strcpy(RetStates.m_szID, pLogin->m_szID);
					
					send(pTemp->m_sSocket, (char *)&RetStates, sizeof(STATE), 0);
				}
			}

			pTemp = pTemp->m_pNext;
		}		
	}

	return 0;
}


int AddApply(DATA_PACK *pData)
{
	ADD *pAddInfo = (ADD *)(pData->m_szBuf);
	USER *pTemp = FindUser(pAddInfo->m_szToID);
	
	if (NULL == pTemp)
	{
		ADD NoAdd;
		
		strcpy(NoAdd.m_szFromID, "无此用户");
		NoAdd.m_ProtoID = ADD_ASW;
		NoAdd.m_nEvent = ADD_NO;
		send(pData->m_pClientIfo->m_sClient, (char *)&NoAdd, sizeof(ADD), 0);
	}
	else
	{
		ADD RetAdd;
		RetAdd.m_ProtoID = ADD_APPLY;
		strcpy(RetAdd.m_szFromID, pAddInfo->m_szFromID);
		strcpy(RetAdd.m_szToID, pAddInfo->m_szToID);
		send(pTemp->m_sSocket, (char *)&RetAdd, sizeof(ADD), 0);
	}
	
	return 0;
}

int AddAsw(DATA_PACK *pData)
{
	ADD *pAddAsw = (ADD *)(pData->m_szBuf);

	if (ADD_OK == pAddAsw->m_nEvent)
	{
		FRIEND RetAddFri;
		
		USER *pMy = FindUser(pAddAsw->m_szFromID);
		USER *pTo = FindUser(pAddAsw->m_szToID);
		
		strcpy(RetAddFri.m_szID, pMy->m_szID);
		strcpy(RetAddFri.m_szName, pMy->m_szName);
		strcpy(RetAddFri.m_szState, pMy->m_szState);
		strcpy(RetAddFri.m_szSex, pMy->m_szSex);
		AddFriNode(&(pTo->m_pFriHead), &(pTo->m_pFriEnd), &RetAddFri);
		
		char szName[30] = "";
		strcpy(szName, pTo->m_szID);
		strcat(szName, "FriList.txt");
		
		FILE *pFile = fopen(szName, "a");
		
		fwrite(&RetAddFri, sizeof(FRIEND), 1, pFile);
		
		fclose(pFile);
		
		RetAddFri.m_ProtoID = ADD_ASW;
		RetAddFri.m_nEvent = ADD_OK;

		send(pTo->m_sSocket, (char *)&RetAddFri, sizeof(FRIEND), 0);
		
		strcpy(RetAddFri.m_szID, pTo->m_szID);
		strcpy(RetAddFri.m_szName, pTo->m_szName);
		strcpy(RetAddFri.m_szState, pTo->m_szState);
		strcpy(RetAddFri.m_szSex, pTo->m_szSex);
		AddFriNode(&(pMy->m_pFriHead), &(pMy->m_pFriEnd), &RetAddFri);		
		
		strcpy(szName, pMy->m_szID);
		strcat(szName, "FriList.txt");
		
		pFile = fopen(szName, "a");
		
		fwrite(&RetAddFri, sizeof(FRIEND), 1, pFile);
		
		fclose(pFile);
		
		RetAddFri.m_ProtoID = ADD_ASW;
		RetAddFri.m_nEvent = ADD_OK;
		send(pMy->m_sSocket, (char *)&RetAddFri, sizeof(FRIEND), 0);
	}
	if (ADD_NO == pAddAsw->m_nEvent)
	{
		USER *pTemp = FindUser(pAddAsw->m_szToID);
		
		if (NULL == pTemp)
		{
			return -1;
		}
		
		EVNET RetAdd;
		RetAdd.m_nProtoID = ADD_ASW;
		RetAdd.m_nEvent = ADD_NO;
		send(pData->m_pClientIfo->m_sClient, (char *)&RetAdd, sizeof(EVNET), 0);
	}
	return 0;
}



int DeleteApply(DATA_PACK *pData)
{			
	DELETES *pFriend = (DELETES *)(pData->m_szBuf);
	
	USER *pMy = FindUser(pFriend->m_szFromID);
	USER *pTo = FindUser(pFriend->m_szToID);
	
	DeleteNode(&pMy->m_pFriHead, &pMy->m_pFriEnd, pTo->m_szID);
	DeleteNode(&pTo->m_pFriHead, &pTo->m_pFriEnd, pMy->m_szID);
	DELETES RetDelete;
	RetDelete.m_ProtoID = DELETE_APPLY;
	RetDelete.m_nEvent = 1;
	strcpy(RetDelete.m_szFromID, pFriend->m_szFromID);
	strcpy(RetDelete.m_szToID, pFriend->m_szToID);
	send(pTo->m_sSocket, (char *)&RetDelete, sizeof(DELETES), 0);
	RetDelete.m_nEvent = 2;
	send(pMy->m_sSocket, (char *)&RetDelete, sizeof(DELETES), 0);
	
	
	char szFriName[30] = "";
	
	strcpy(szFriName, pMy->m_szID);
	strcat(szFriName, "FriList.txt");
	
	FILE *fpDF = fopen(szFriName, "w+");
	
	FRIEND *pTemp = pMy->m_pFriHead;
	
	while (NULL != pTemp)
	{
		fwrite(pTemp, sizeof(FRIEND), 1, fpDF);
		
		pTemp = pTemp->m_pNext;
	}
	
	fclose(fpDF);
	
	
	strcpy(szFriName, pTo->m_szID);
	strcat(szFriName, "FriList.txt");
	
	FILE *fpDFd = fopen(szFriName, "w+");
	
	pTemp = pTo->m_pFriHead;
	
	while (NULL != pTemp)
	{	
		fwrite(pTemp, sizeof(FRIEND), 1, fpDF);
		
		pTemp = pTemp->m_pNext;
	}
	fclose(fpDFd);
	
	return 0;
}



int RefreshApply(DATA_PACK *pData)
{
	STATE *pMyState = (STATE *)(pData->m_szBuf);
	
	USER *pTemp = g_pUserHead;
	
	while (NULL != pTemp)
	{
		FRIEND *pFri = FindNode(pTemp->m_pFriHead, pMyState->m_szID);
		
		if (NULL == pFri)
		{
			pTemp = pTemp->m_pNext;
			continue;
		}
		if (0 != strcmp(pMyState->m_szState, pFri->m_szState))
		{
			strcpy(pFri->m_szState, pMyState->m_szState);
	
			STATE RetStates;
			
			RetStates.m_ProtoID = REFRESH_ASW;
			RetStates.m_nEvent = REFRESH_OK;
			strcpy(RetStates.m_szState, pMyState->m_szState);
			strcpy(RetStates.m_szID, pMyState->m_szID);
			
			send(pTemp->m_sSocket, (char *)&RetStates, sizeof(STATE), 0);
			
		}

		pTemp = pTemp->m_pNext;
	}
	
	return 0;
}



int UserInfo(DATA_PACK *pData)
{
	STATE *pState = (STATE *)(pData->m_szBuf);
	USER *pUser = FindUser(pState->m_szID);
	
	if(NULL == pUser)
	{
		return 0;
	}
	
	REGIS MyInfo;
	
	MyInfo.m_ProtoID = USER_INFO;
	
	strcpy(MyInfo.m_szID, pUser->m_szID);
	strcpy(MyInfo.m_szName, pUser->m_szName);
	strcpy(MyInfo.m_szSex, pUser->m_szSex);
	strcpy(MyInfo.m_szAge, pUser->m_szAge);
	strcpy(MyInfo.m_szEmail, pUser->m_szEmail);
	
	send(pData->m_pClientIfo->m_sClient, (char *)&MyInfo, sizeof(REGIS), 0);
	
	return 0;
}

int MessageSend(DATA_PACK *pData)
{
	NEWS *pNewsSend = (NEWS *)(pData->m_szBuf);
	USER *pTemp = FindUser(pNewsSend->m_szToID);
	
	if (0 == pNewsSend->m_Frist)
	{	
		NEWS RetNew;
		RetNew.m_ProtoID = MESSAGE_SEND;
		RetNew.m_nEvent = NEWSMESSAGE;
		RetNew.m_Frist = 1;
		
		strcpy(RetNew.m_szFromID, pNewsSend->m_szFromID);
		strcpy(RetNew.m_szToID, pNewsSend->m_szToID);
		strcpy(RetNew.m_szMessage, pNewsSend->m_szMessage);
		send(pTemp->m_sSocket, (char *)&RetNew, sizeof(RetNew), 0);
	}
	
	NEWS RetNew;
	RetNew.m_ProtoID = MESSAGE_RECV;
//	RetNew.m_nEvent = NEWSMESSAGE;

	strcpy(RetNew.m_szFromID, pNewsSend->m_szFromID);
	strcpy(RetNew.m_szToID, pNewsSend->m_szToID);
	strcpy(RetNew.m_szMessage, pNewsSend->m_szMessage);
	send(pTemp->m_sSocket, (char *)&RetNew, sizeof(RetNew), 0);
	
	return 0;
}

int MessageRecv(DATA_PACK *pData)
{
	NEWS *pNewsSend = (NEWS *)(pData->m_szBuf);
	USER *pTemp = FindUser(pNewsSend->m_szToID);
	
	if (0 == pNewsSend->m_Frist)
	{	
		NEWS RetNew;
		RetNew.m_ProtoID = MESSAGE_SEND;
		RetNew.m_nEvent = NEWSMESSAGE;
		RetNew.m_Frist = 1;

		strcpy(RetNew.m_szFromID, pNewsSend->m_szFromID);
		strcpy(RetNew.m_szToID, pNewsSend->m_szToID);
		strcpy(RetNew.m_szMessage, pNewsSend->m_szMessage);
		send(pTemp->m_sSocket, (char *)&RetNew, sizeof(RetNew), 0);
	}
	
	NEWS RetNew;
	RetNew.m_ProtoID = MESSAGE_RECV;
//	RetNew.m_nEvent = NEWSMESSAGE;

	strcpy(RetNew.m_szFromID, pNewsSend->m_szFromID);
	strcpy(RetNew.m_szToID, pNewsSend->m_szToID);
	strcpy(RetNew.m_szMessage, pNewsSend->m_szMessage);
	send(pTemp->m_sSocket, (char *)&RetNew, sizeof(RetNew), 0);
	
	return 0;
}

