#include "StdAfx.h"
#include "ThreadFunc.h"
#include "resource.h"
#include "FriList.h"

HWND hDlgMain;

PRO_MAP g_ProMap[] = {{REG_APPLY, RegApply},
					{REG_ASW, RegAsw},
					{LOGIN_APPLY, LoginApply},
					{LOGIN_ASW, LoginAsw},
					{ADD_APPLY, AddApply}, 
					{ADD_ASW, AddAsw},
					{CHAT_FRIEND, ChatFriend},
					{DELETE_APPLY, DeleteApply}, 
					{DELETE_ASW, DeleteAsw}, 
					{REFRESH_APPLY, RefreshApply},
					{REFRESH_ASW, RefreshAsw}, 
					{USER_INFO, UserInfo},
					{MESSAGE_RECV, MessageRecv},
					{MESSAGE_SEND, MessageSend},
					{0, NULL}};


DWORD WINAPI RecvThread(LPVOID lpLparam)
{
	hDlgMain = (HWND)lpLparam;
	DATA_PACK *pDate = NULL;
	
	while (TRUE)
	{
		pDate = new DATA_PACK;
		
		if (SOCKET_ERROR == recv(g_sCLient, pDate->m_szBuf, 512, 0))
		{
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

	return 0;
}

int RegAsw(DATA_PACK *pData)
{
	
	return 0;
}

int LoginApply(DATA_PACK *pData)
{
	
	return 0;
}

int LoginAsw(DATA_PACK *pData)
{
	
	return 0;
}

int AddApply(DATA_PACK *pData)
{
	ADD add = *(ADD*)(pData->m_szBuf);
	char szAddInfo[20];
	
	strcpy(szAddInfo, add.m_szFromID);
	strcat(szAddInfo, "申请添加好友");
	strcpy(g_FriID, add.m_szFromID);
	
	int nResult = MessageBox(hDlgMain, szAddInfo, "AddFriend", MB_YESNO);
	if (IDYES == nResult)
	{
		ADD add_asw;
		strcpy(add_asw.m_szFromID, g_pFriHead->m_szID);
		strcpy(add_asw.m_szToID, g_FriID);
		
		add_asw.m_ProtoID = ADD_ASW;
		add_asw.m_nEvent = ADD_OK;
		
		send(g_sCLient, (char *)&add_asw, sizeof(ADD), 0);
	}
	if (IDNO == nResult)
	{
		ADD add_asw;
		strcpy(add_asw.m_szFromID, g_pFriHead->m_szID);
		strcpy(add_asw.m_szToID, g_FriID);
		
		add_asw.m_ProtoID = ADD_ASW;
		add_asw.m_nEvent = ADD_NO;
		
		send(g_sCLient, (char *)&add_asw, sizeof(ADD), 0);
	}
	
	return 0;
}

int AddAsw(DATA_PACK *pData)
{
	FRIEND Friend = *(FRIEND *)(pData->m_szBuf);

	if (ADD_OK == Friend.m_nEvent)
	{
		char szAddInfo[20];
		
		strcpy(szAddInfo, Friend.m_szID);
		strcat(szAddInfo, "同意添加好友");
		MessageBox(hDlgMain, szAddInfo, "AddFriend", MB_OK);
		AddFriNode(&g_pFriHead, &g_pFriEnd, &Friend);
		
		char szName[50];
		
		sprintf(szName, "%s  %s  %s", Friend.m_szID, Friend.m_szName, Friend.m_szState);
		SendDlgItemMessage(hDlgMain, IDL_FRIEND, LB_ADDSTRING, 0, (LPARAM)szName);
	}
	if (ADD_NO == Friend.m_nEvent)
	{
		ADD add_asw = *(ADD *)(pData->m_szBuf);
		char szAddInfo[20];
		
		if (0 == strcmp(add_asw.m_szFromID, "无此用户"))
		{
			strcpy(szAddInfo, add_asw.m_szFromID);
			MessageBox(hDlgMain, szAddInfo, "LALALA", MB_OK);
		}
		else
		{
			strcpy(szAddInfo, add_asw.m_szFromID);
			strcat(szAddInfo, "拒绝添加好友");
			MessageBox(hDlgMain, szAddInfo, "LALALA", MB_OK);
		}	
	}

	return 0;
}

int ChatFriend(DATA_PACK *pData)
{	
	FRIEND *pNew = (FRIEND *)(pData->m_szBuf);
	
	int nSize = pNew->m_nEvent;
	char szName[50];

	for(int i = 0; nSize > 0; ++i, nSize--)
	{
		AddFriNode(&g_pFriHead, &g_pFriEnd, &(pNew[i]));

		sprintf(szName, "%s  %s  %s", pNew[i].m_szID, pNew[i].m_szName, pNew[i].m_szState);
		SendDlgItemMessage(hDlgMain, IDL_FRIEND, LB_ADDSTRING, 0, (LPARAM)szName);
	}
	
	strcpy(g_MyInfo.m_szID, pNew->m_szID);
	strcpy(g_MyInfo.m_szName, pNew->m_szID);
	strcpy(g_MyInfo.m_szSex, pNew->m_szSex);
	strcpy(g_MyInfo.m_szState, "在线");
	
	SetWindowText(hDlgMain, g_MyInfo.m_szName);
	SetWindowText(GetDlgItem(hDlgMain, IDS_SID), g_MyInfo.m_szID);
	SendDlgItemMessage(hDlgMain, IDR_ONLINE, BM_CLICK, 0, 0);
	
	return 0;
}

int DeleteApply(DATA_PACK *pData)
{
	DELETES Delete_apply = *(DELETES *)(pData->m_szBuf);
	FRIEND *pTemp;
	
	if (1 == Delete_apply.m_nEvent)
	{
		pTemp = FindNode(g_pFriHead, Delete_apply.m_szFromID);
	}
	if (2 == Delete_apply.m_nEvent)
	{
		pTemp = FindNode(g_pFriHead, Delete_apply.m_szToID);
	}

	SendDlgItemMessage(hDlgMain, IDL_FRIEND, LB_DELETESTRING, (WPARAM)(pTemp->m_nCount), 0);	

	DeleteNode(&g_pFriHead, &g_pFriEnd, pTemp->m_szID);
	
	return 0;
}

int DeleteAsw(DATA_PACK *pData)
{
	
	return 0;
}

int RefreshApply(DATA_PACK *pData)
{
	
	return 0;
}

int RefreshAsw(DATA_PACK *pData)
{
	FRIEND *pTemp = g_pFriHead;

	STATE state = *(STATE *)(pData->m_szBuf);
	if (state.m_nEvent == REFRESH_OK)
	{
		FRIEND *pFri = FindNode(g_pFriHead, state.m_szID);
		char szFriend[50];

		strcpy(pFri->m_szState, state.m_szState);
		sprintf(szFriend, "%s  %s  %s", pFri->m_szID, pFri->m_szName, pFri->m_szState);
		SendDlgItemMessage(hDlgMain, IDL_FRIEND, LB_DELETESTRING, (WPARAM)(pFri->m_nCount), 0);
		SendDlgItemMessage(hDlgMain, IDL_FRIEND, LB_INSERTSTRING, (WPARAM)(pFri->m_nCount), (LPARAM)szFriend);
	}
	
	return 0;
}

int UserInfo(DATA_PACK *pData)
{
	char szBuf[128] = "";
	REGIS MyInfo = *(REGIS *)(pData->m_szBuf);
	
	sprintf(szBuf, "ID: %s\nName :%s\nSex :%s\nAge :%s\nE-Mail :%s\n", MyInfo.m_szID, MyInfo.m_szName, MyInfo.m_szSex, MyInfo.m_szAge, MyInfo.m_szEmail);
	MessageBox(hDlgMain, szBuf, "Info", MB_OK);
	
	return 0;
}

int MessageSend(DATA_PACK *pData)
{
	NEWS FriNew = *(NEWS *)(pData->m_szBuf);

	if (NEWSMESSAGE == FriNew.m_nEvent)
	{	
		char szNewFrom[255] = "";
		FRIEND *pFri = FindNode(g_pFriHead, FriNew.m_szFromID);
		
		strcpy(szNewFrom, FriNew.m_szFromID);
		strcat(szNewFrom, "的新消息!");
		MessageBox(hDlgMain, szNewFrom, "Chat", MB_OK);

//		SendDlgItemMessage(hDlgMain, IDL_FRIEND, LB_SETITEMDATA, (WPARAM)(pFri->m_nCount), (LPARAM)1);
//		InvalidateRect(hDlgMain, NULL, TRUE);
		strcpy(g_FriID, FriNew.m_szFromID);
		strcpy(szNewFrom, NewSend.m_szFromID);
		strcat(szNewFrom, " :");
		strcat(szNewFrom, NewSend.m_szMessage);
		strcpy(NewSend.m_szMessage, szNewFrom);
		DialogBox((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CHAR), hDlgMain, ChatProc);
	}
	
	return 0;
}

int MessageRecv(DATA_PACK *pData)
{
	char szMessage[255] = "";

	NewSend = *(NEWS *)(pData->m_szBuf);
	strcpy(szMessage, NewSend.m_szFromID);
	strcat(szMessage, " :");
	strcat(szMessage, NewSend.m_szMessage);
	strcpy(g_FriID, NewSend.m_szFromID);
	SendDlgItemMessage(hWndChat, IDL_CHAT, LB_ADDSTRING, 0, (LPARAM)szMessage);
//	FRIEND *pFri = FindNode(g_pFriHead, NewSend.m_szFromID);
//	SendDlgItemMessage(hDlgMain, IDL_FRIEND, LB_SETITEMDATA, (WPARAM)(pFri->m_nCount), (LPARAM)1);
//	InvalidateRect(hDlgMain, NULL, TRUE);
	strcpy(NewSend.m_szMessage, szMessage);

	return 0;
}

