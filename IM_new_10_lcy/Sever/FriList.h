#define __FRILIST_H__
#define __FRILIST_H__

struct FRIEND
{
	char m_szID[11];
	char m_szName[20];
	char m_szState[6];

	FRIEND *m_pPre;
	FRIEND *m_pNext;
};

int AddFriNode(FRIEND **ppHead, FRIEND **ppEnd, FRIEND *pNode);
FRIEND *FindNode(FRIEND *pHead, char *pID);
int DeleteNode(FRIEND **ppHead, FRIEND **ppEnd,char *pID);
int ClearList(FRIEND **ppHead, FRIEND **ppEnd,);

extern FRIEND *g_pFriHead;
extern FRIEND *g_pFriEnd;

#define endif