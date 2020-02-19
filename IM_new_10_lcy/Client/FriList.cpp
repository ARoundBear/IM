#include "Stdafx.h"
#include "FriList.h"

FRIEND *g_pFriHead;
FRIEND *g_pFriEnd;

int AddFriNode(FRIEND **ppHead, FRIEND **ppEnd, FRIEND *pNode)
{
	if(NULL == pNode)
	{
		return -1;
	}

	FRIEND *pNew = new FRIEND;

	pNew->m_pNext = NULL;
	pNew->m_pPre = NULL;
	strcpy(pNew->m_szID, pNode->m_szID);
	strcpy(pNew->m_szName, pNode->m_szName);
	strcpy(pNew->m_szState, pNode->m_szState);
	strcpy(pNew->m_szSex, pNode->m_szSex);

	if(NULL == *ppEnd)
	{
		*ppHead = pNew;
		*ppEnd = pNew;
		pNew->m_nCount = 0;
	}
	else
	{
		pNew->m_nCount = (*ppEnd)->m_nCount +1;	
		(*ppEnd)->m_pNext = pNew;
		pNew->m_pPre = *ppEnd;

		*ppEnd = pNew;

	}
	
	return 0;
}

FRIEND *FindNode(FRIEND *pHead,char *pID)
{
	if(NULL == pID || NULL == pHead)
	{
		return NULL;
	}

	FRIEND *pTemp = pHead;

	while(NULL != pTemp)
	{
		if(0 == strcmp(pID, pTemp->m_szID))
		{
			break;
		}

		pTemp = pTemp->m_pNext;
	}
	
	return pTemp;
}


int SetNum(FRIEND *pPos);

int DeleteNode(FRIEND **ppHead, FRIEND **ppEnd,char *pID)
{
	if(NULL == pID || NULL == *ppHead)
	{
		return -1;
	}

	FRIEND *pPos = FindNode(*ppHead, pID);


	if(NULL == pPos)
	{
		return -1;
	}

	SetNum(pPos);

	if(pPos == *ppHead)
	{
		*ppHead = pPos->m_pNext;

		if(NULL == pPos->m_pNext)
		{
			*ppEnd = NULL;
		}
		pPos->m_pNext->m_pPre = NULL;
	}
	else
	{
		if(pPos != *ppEnd)
		{
			pPos->m_pNext->m_pPre = pPos->m_pPre;
			pPos->m_pPre->m_pNext = pPos->m_pNext;
		}
		else
		{
			pPos->m_pPre->m_pNext = NULL;
			*ppEnd = pPos->m_pPre;
		}

	}
	
	delete pPos;

	return 0;
}

int ClearList(FRIEND **ppHead, FRIEND **ppEnd)
{
	FRIEND *pNext = NULL;

	while(NULL != *ppHead)
	{
		pNext = (*ppHead)->m_pNext;
		delete *ppHead;
		*ppHead = pNext;
	}

	*ppEnd = NULL;

	return 0;
}

int SetNum(FRIEND *pPos)
{
	while (NULL != pPos->m_pNext)
	{
		pPos->m_pNext->m_nCount = pPos->m_nCount;

		pPos = pPos->m_pNext;
	}

	return 0;
}

