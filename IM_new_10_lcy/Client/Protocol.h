#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

struct PROTO
{
	int m_nEvent;
	int m_nSize;
	char m_szData[];
};

struct REGIS
{
	int m_ProtoID;
	int m_nEvent;
	char m_szID[11];
	char m_szPsd[13];
	char m_szName[20];
	char m_szSex[4];
	char m_szAge[4];
	char m_szEmail[20];
};

struct LOGIN
{
	int m_ProtoID;
	int m_nEvent;
	char m_szID[11];
	char m_szPsd[13];
	char m_szState[6];
};

typedef struct ADD
{
	int m_ProtoID;
	int m_nEvent;
	char m_szFromID[11];
	char m_szToID[11];
}DELETES;

struct Message
{
	int m_ProtoID;
	int m_nEvent;
	char m_szFromID[11];
	char m_szToID[11];
	char m_szMessage[255];
	bool m_bFirst;
};

struct STATE 
{
	int m_ProtoID;
	int m_nEvent;
	char m_szID[11];
	char m_szState[6];
};


struct NEWS
{
	int m_ProtoID;
	int m_nEvent;
	char m_szFromID[10];
	char m_szToID[10];
	char m_szMessage[150];
	int m_Frist;
};

struct EVNET
{
	int m_nProtoID;
	int m_nEvent;
};

#define REG_APPLY 1
#define REG_ASW 2
#define REG_OK 3
#define REG_NO 4
#define LOGIN_APPLY 5
#define LOGIN_ASW 6
#define LOGIN_OK 7
#define LOGIN_NO 8
#define ADD_APPLY 9
#define ADD_ASW 10
#define ADD_OK 11
#define ADD_NO 12
#define CHAT_FRIEND 13
#define DELETE_APPLY 14
#define DELETE_ASW 15
#define DELETE_OK 16
#define DELETE_NO 17
#define FRIEND_END 18
#define REFRESH_APPLY 19
#define REFRESH_ASW 20
#define REFRESH_OK 21
#define USER_INFO 22
#define MESSAGE_SEND 23
#define MESSAGE_RECV 24
#define NEWSMESSAGE 25

#endif