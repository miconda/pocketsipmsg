// SIPMsg.cpp: implementation of the CSIPMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PocketSM.h"
#include "SIPMsg.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/** not case comparison between two strings */
#define char_cmp(x,y) ((x) == (y) ? 0 : ((x) > (y) ? 1 : -1))
/**
 * case insensitive str cmp
 */
int m_strncasecmp(char *a, char *b, int l)
{
	unsigned char c1, c2;
	int i;

	for(i=0; i<l; i++)
	{
		if (*a == '\0' || *b == '\0')
			return char_cmp(*a,*b);
		c1= (isascii(*a) && isupper(*a)) ? tolower(*a) : *a;
		c2= (isascii(*b) && isupper(*b)) ? tolower(*b) : *b;
		if (c1 != c2)
			return char_cmp(c1,c2);
		a++;
		b++;
	}
	return 0;
}
/***
char *strstr(char const *haystack, char const *needle)
{
	while (haystack[0]!=0)
	{
		int i=0;
		while (haystack[i]==needle[i] && haystack[i]!=0 && needle[i]!=0)
			i++;
		if (needle[i]==0)
			return (char *)haystack;

		haystack++;
	}
	return (char *)(NULL);
}
***/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSIPMsg::CSIPMsg()
{
	memset(&sip_msg, 0, sizeof(sip_msg));
	memset(&from, 0, sizeof(from));
	memset(&body, 0, sizeof(body));
}

CSIPMsg::~CSIPMsg()
{
	if(sip_msg.s != NULL)
		free(sip_msg.s);
}

/**
 * set the unparsed message
 */
int CSIPMsg::setSIPMsg(char* msg, int len)
{
	if(msg == NULL)
		return -1;
	if(len > 0)
	{
		sip_msg.len = len;
		msg[len] = 0;
	}
	else
		sip_msg.len = strlen(msg);
	
	if(sip_msg.s != NULL)
		free(sip_msg.s);

	sip_msg.s = _strdup(msg);
	memset(&from, 0, sizeof(from));
	memset(&body, 0, sizeof(body));
	return 0;
}

/**
 * get message's 'from'
 */
int CSIPMsg::getFrom(str *f)
{
	char *p, *p1, *p2;
	if(from.s != 0 && from.len >= 0)
	{
		f->len = from.len;
		f->s = from.s;
		return 0;
	}
	if(f == NULL)
		return -1;
	if(sip_msg.s == NULL || sip_msg.len == 0)
		return -1;
	if((p = strstr(sip_msg.s, "From:")) == NULL)
		if((p = strstr(sip_msg.s, "from:")) == NULL)
			if((p = strstr(sip_msg.s, "FROM:")) == NULL)
				return -1;

	p += 5;
	while(*p==' ' || *p=='\t') p++;

	p1 = strchr(p, '\n');
	if(p1 == NULL)
		return -1;
	while(*p1==' ' || *p1=='\t' || *p1=='\r' || *p1=='\n') p1--;

	p2 = strchr(p, ';');
	if(p2 == NULL)
		return -1;
	while(*p2==' ' || *p2=='\t' || *p2==';') p2--;

	if(p2 < p1)
		p1 = p2;

	f->s = p;
	f->len = p1 - p +1;

	return 0;
}

/**
 * get message's body
 */
int CSIPMsg::getBody(str *b)
{
	char *p;
	int cLen = 0;

	if(body.s != 0 && body.len >= 0)
	{
		b->len = body.len;
		b->s = body.s;
		return 0;
	}
	if(b == NULL)
		return -1;
	if(sip_msg.s == NULL || sip_msg.len == 0)
		return -1;
	
	if((p = strstr(sip_msg.s, "Content-Length:")) == NULL)
		if((p = strstr(sip_msg.s, "Content-length:")) == NULL)
			if((p = strstr(sip_msg.s, "content-length:")) == NULL)
				if((p = strstr(sip_msg.s, "CONTENT-LENGTH:")) == NULL)
					return -1;

	p += 15;
	cLen = atoi(p);
	if(cLen <= 0)
		return -1;
	b->s = sip_msg.s + sip_msg.len - cLen;
	b->len = cLen;
	body.len = b->len;
	body.s = b->s;

	return 0;
}

/**
 * check if is MESSAGE
 */
int CSIPMsg::isIdentic(char *msg, int len)
{
	if(sip_msg.s == NULL || sip_msg.len == 0 || sip_msg.len != len)
		return 0;
	if(strncmp(sip_msg.s, msg, len) == 0)
		return 1;
	return 0;
}

/**
 * buld message response
 */
int CSIPMsg::buildResponse(str *r, char *fl, CString cAddr, int cPort)
{
	char *p, *p0, *p1, *end;
	int i;
	CString strPort;

	if(r==NULL || r->s==NULL || r->len<=0 || fl==NULL 
			|| sip_msg.s==NULL || sip_msg.len==0)
		return -1;
	
	memset(r->s, 0, r->len);
	p = r->s;
	strcpy(p, fl);
	p += strlen(fl);
	//skip first line
	p0 = strchr(sip_msg.s, '\n');
	if(p0 == NULL)
		return -1;
	p0++;
	end = strstr(sip_msg.s, "\r\n\r\n");
	if(end == NULL)
	{
		end = strstr(sip_msg.s, "\n\n");
		if(end == NULL)
			return -1;
	}
	while(p0 < end)
	{
		p1 = strchr(p0, '\n');
		if(p1 == NULL)
			return -1;
		if(!m_strncasecmp(p0, "Via:", 4))
		{
			strncpy(p, p0, p1-p0);
			p += p1-p0;
			if(*(p-1) != '\r')
				*p++ = '\r';
			*p++ = '\n';
		}
		else if(!m_strncasecmp(p0, "From:", 5))
		{
			strncpy(p, p0, p1-p0);
			p += p1-p0;
			if(*(p-1) != '\r')
				*p++ = '\r';
			*p++ = '\n';
		}
		else if(!m_strncasecmp(p0, "To:", 3))
		{
			strncpy(p, p0, p1-p0);
			p += p1-p0;
			if(*(p-1) != '\r')
				*p++ = '\r';
			*p++ = '\n';
		}
		else if(!m_strncasecmp(p0, "Call-ID:", 8))
		{
			strncpy(p, p0, p1-p0);
			p += p1-p0;
			if(*(p-1) != '\r')
				*p++ = '\r';
			*p++ = '\n';
		}
		else if(!m_strncasecmp(p0, "CSeq:", 5))
		{
			strncpy(p, p0, p1-p0);
			p += p1-p0;
			if(*(p-1) != '\r')
				*p++ = '\r';
			*p++ = '\n';
		}
		p0 = p1 + 1;
	}

	strncpy(p, "Contact: <", 10);
	p += 10;
	for(i=0; i< cAddr.GetLength(); i++)
		*p++ = (char)(cAddr.GetAt(i) & 0xFF);
	strPort.Format(_T(":%d"), cPort);
	for(i=0; i< strPort.GetLength(); i++)
		*p++ = (char)(strPort.GetAt(i) & 0xFF);
	*p++ = '>';
	*p++ = '\r';
	*p++ = '\n';
	strncpy(p, "Content-Length: 0\r\n\r\n", 21);
	p += 21;
	*p = 0;
	r->len = p - r->s;

	return 0;
}
