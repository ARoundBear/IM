#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#include <winsock2.h>

struct CLIENT_INFO
{
	SOCKET m_sClient;
	sockaddr_in m_addrClient;
};

struct DATA_PACK
{
	CLIENT_INFO *m_pClientIfo;
	char m_szBuf[512];
};


#endif