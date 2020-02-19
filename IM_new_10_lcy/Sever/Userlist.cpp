#include "StdAfx.h"
#include "UserList.h"

USER *g_pUserHead = NULL;
USER *g_pUserEnd = NULL;

int AddNode(USER *pNode)
{
	if(NULL == pNode)
	{
		return -1;
	}

	if(NULL == g_pUserHead)
	{
		g_pUserHead = pNode;
		g_pUserEnd = pNode;
	}
	else
	{
		g_pUserEnd->m_pNext = pNode;
		pNode->m_pPre = g_pUserEnd;

		g_pUserEnd = pNode;
	}
	
	return 0;
}

USER *FindUser(const char *pID)
{
	if (NULL == pID || NULL == g_pUserHead)
	{
		return NULL;
	}

	USER *pTemp = g_pUserHead;

	while (NULL != pTemp)
	{
		if(0 == strcmp(pID, pTemp->m_szID))
		{
			break;
		}

		pTemp = pTemp->m_pNext;
	}
	
	return pTemp;
}

int LoadDate()
{
	FILE *pFile = fopen("AllUserInfo.txt", "a+");

	if(NULL == pFile)
	{
		cout<<"open fail!";

		return -1;
	}


	while (!feof(pFile))
	{
		USER *pNew = new USER;
		
		fread(pNew, sizeof(USER), 1, pFile);

		if(feof(pFile))
		{
			delete pNew;
			break;
		}

		pNew->m_pNext = NULL;
		pNew->m_pPre = NULL;
		AddNode(pNew);		
	}	



	fclose(pFile);

	USER *pTemp = g_pUserHead;
	char szBuf[30] = "";

	while(NULL != pTemp)
	{
		sprintf(szBuf, "%sFriList.txt", pTemp->m_szID);

		FILE *pFile = fopen(szBuf, "r");

		while (!feof(pFile))
		{
			FRIEND *pNew = new FRIEND;

			fread(pNew, sizeof(FRIEND), 1, pFile);
			pNew->m_pNext = NULL;
			pNew->m_pPre = NULL;

			if(feof(pFile))
			{
				delete pNew;
				break;
			}
			AddFriNode(&(pTemp->m_pFriHead), &(pTemp->m_pFriEnd), pNew);
		}
	

		pTemp= pTemp->m_pNext;

		fclose(pFile);
	}
	
	return 0;
}

int SaveDate()
{	
	FILE *pFile = fopen("AllUserInfo.txt", "w+");

	if(NULL == pFile)
	{
		cout<<"open fail!";
		
		return -1;
	}

	USER *pTemp = g_pUserHead;

	while (NULL != pTemp)
	{
		fwrite(pTemp, sizeof(USER), 1, pFile);

		pTemp = pTemp->m_pNext;
	}

	fclose(pFile);
	
	return 0;
}


