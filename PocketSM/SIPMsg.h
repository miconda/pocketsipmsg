// SIPMsg.h: interface for the CSIPMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIPMSG_H__DA737839_9263_4A9B_B1F5_03796BB64964__INCLUDED_)
#define AFX_SIPMSG_H__DA737839_9263_4A9B_B1F5_03796BB64964__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _str
{
	char *s;
	int len;
} str;

int m_strncasecmp(char *a, char *b, int l);

class CSIPMsg  
{
public:
	CSIPMsg();
	virtual ~CSIPMsg();

protected:
	str sip_msg;	// the SIP message
	str from;		// from header
	str body;		// body of the message
public:
	int setSIPMsg(char* msg, int len);
	int getFrom(str *f);
	int getBody(str *b);
	int buildResponse(str *r, char *fl, CString cAddr, int cPort);
	int isIdentic(char *msg, int len);
};

#endif // !defined(AFX_SIPMSG_H__DA737839_9263_4A9B_B1F5_03796BB64964__INCLUDED_)
