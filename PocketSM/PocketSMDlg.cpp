// PocketSMDlg.cpp : implementation file 
//

#include "stdafx.h"
#include "PocketSM.h"
#include "PocketSMDlg.h"
#include <stdio.h>
#include <Mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TIMER_A		2001
#define TIMER_A_PERIOD	400
#define RET_MILISEC		500
#define MAX_CONTACTS	10

////////////////////////////////////////////////////////////////////////////
// CPocketSMDlg dialog

/**
 * class constructor
 */
CPocketSMDlg::CPocketSMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPocketSMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPocketSMDlg)
	m_strEditView = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nIPPort = 5060;
	m_pCSock = new CAsyncSocket();
	m_strIPAddress = GetIP();
	m_strUserName = "";
	m_strSipSrvAddress = "";
	m_nSipSrvPort = 5060;
	m_nFlag = m_nCounter = 0;
	m_bDlgHide = false;
	m_bTrayIcon = false;
	m_bSetupOK = false;
	m_dlgSetup = new CSetupDlg(this);
	size=first=last=0;
	MSG_LIST_SIZE = 10;
	m_strEditMsg = _T("");
}

/**
 * data exchange between controls and variables
 */
void CPocketSMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPocketSMDlg)
	DDX_Control(pDX, IDC_EDIT_MSG, m_ctlEditMsg);
	DDX_Control(pDX, IDC_COMBO_TO, m_ctlComboTo);
	DDX_Text(pDX, IDC_EDIT_VIEW, m_strEditView);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPocketSMDlg, CDialog)

	ON_MESSAGE(WM_ICON_NOTIFY,OnIconNotify) // macro for message handler !!

	//{{AFX_MSG_MAP(CPocketSMDlg)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_SETUP, OnButtonSetup)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_BN_CLICKED(ID_BUTTON_HIDE, OnButtonHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPocketSMDlg message handlers

/**
 * init dialog
 */
BOOL CPocketSMDlg::OnInitDialog()
{
	char userName[70], srvAddress[50];
	int srvPort;
	CString lStr;
	FILE *f = NULL;

	CDialog::OnInitDialog();

	DWORD nonblock = 1;

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CenterWindow(GetDesktopWindow());	// center to the hpc screen

	// TODO: Add extra initialization here

	m_ctlComboTo.LimitText(100);
	if((f = fopen("psm.cfg", "rt")) != NULL)
	{
		if(fscanf(f, "%s%s%d", userName, srvAddress, &srvPort) == 3)
		{
			m_strUserName = userName;
			m_strSipSrvAddress = srvAddress;
			m_nSipSrvPort = srvPort;
			m_bSetupOK = true;

			while(fscanf(f, "%s", userName) == 1)
			{
				lStr = userName;
				m_ctlComboTo.AddString(lStr);
			}
		}
		fclose(f);
	}
	m_dlgSetup->setUserName(m_strUserName);
	m_dlgSetup->setServerAddress(m_strSipSrvAddress);
	m_dlgSetup->setServerPort(m_nSipSrvPort);

	//m_strEditView = "Creating socket ...";
	//UpdateData(FALSE);

	ASSERT( m_pCSock == NULL );
	if(m_pCSock->Create(5060, SOCK_DGRAM) == 0)
	{
		AfxMessageBox(_T("Can not create the socket! Please restart."));
	} 
	m_pCSock->IOCtl(FIONBIO, &nonblock);

	SetTimer(ID_TIMER_A, TIMER_A_PERIOD, 0);

	//m_strEditView.Format(_T("Listening on: %s:%d"), 
	//	m_strIPAddress, m_nIPPort);
	//UpdateData(FALSE);

	AddSystemTrayIcon();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/**
 * send REGISTER packet to SIP server
 */
int CPocketSMDlg::SendSIPRegister(char* expires, int nr)
{
	CString strBuf, strPort;
	CString servAddress;
	UINT servPort;
	char outBuf[2048], pBuf[1024];
	int i, l, n;
	strPort.Format(_T("%d"), m_nIPPort);

	strBuf="REGISTER sip:";
	strBuf += m_strSipSrvAddress;

	strBuf += " SIP/2.0\r\nVia: SIP/2.0/UDP "+m_strIPAddress+":"+strPort;
    strBuf += "\r\nFrom: sip:"+m_strUserName;
    strBuf += "\r\nTo: sip:"+m_strUserName;
    strBuf += "\r\nCall-ID: 70710pocketpc@"+m_strIPAddress;
    strBuf += "\r\nCSeq: 1 REGISTER\r\nContact: <sip:"
		+m_strIPAddress+":"+strPort+">\r\nExpires: ";
	strBuf += expires;
    strBuf += "\r\nUser-Agent: PocketSipM v0.1\r\nContent-Length: 0\r\n\r\n";

	// convert from UNICODE to ASCII
	l = strBuf.GetLength();
	for(i=0; i<l; i++)
		outBuf[i] = (char)(strBuf.GetAt(i) & 0xFF);
	
	n = 1;
	while(n < nr+1)
	{
		i = m_pCSock->SendTo(outBuf, l, m_nSipSrvPort, (LPCTSTR)m_strSipSrvAddress);
		if(i == SOCKET_ERROR || i != l)
			return -1;
		Sleep(RET_MILISEC);
		i = m_pCSock->ReceiveFrom((void*)pBuf, 1023, servAddress, servPort);
		if(i > 0)
		{
			pBuf[i] = 0;
			if(!strncmp(pBuf, "SIP/2.0 ", 8))
			{
				if(*(pBuf+8) == '2')
					return 0;
				if(*(pBuf+8) == '3' || *(pBuf+8) == '4' 
						|| *(pBuf+8) == '5' || *(pBuf+8) == '6')
					return -1;
			}
		}
		n++;
	}
	return -1;
}

/**
 * send MESSAGE packet to SIP server
 */
int CPocketSMDlg::SendSIPMessage(CString to, CString body, int nr)
{
	CString strBuf, strPort, callID;
	CString servAddress;
	UINT servPort;
	char outBuf[2048], pBuf[1024];
	int i, l, n;
	CTime t = CTime::GetCurrentTime();

	strPort.Format(_T("%d"), m_nIPPort);
	callID.Format(_T("8923pPC@%s.%08lX"), m_strIPAddress, t.GetTime());

	strBuf="MESSAGE sip:";
	strBuf += to;

	strBuf += " SIP/2.0\r\nVia: SIP/2.0/UDP "+m_strIPAddress+":"+strPort;
    strBuf += "\r\nFrom: sip:"+m_strUserName;
    strBuf += ";tag=a8437-3mkdsf-9474kjf-2323\r\nTo: sip:"+to;
    strBuf += "\r\nCall-ID: "+callID;
    strBuf += "\r\nCSeq: 1 MESSAGE\r\nContact: <sip:"
		+m_strUserName+">\r\nContent-Type: text/plain; charset=UTF-8";
    strBuf += "\r\nUser-Agent: PocketSipM v0.1\r\nContent-Length: ";
	strPort.Format(_T("%d"), body.GetLength());
	strBuf += strPort+"\r\n\r\n";
	strBuf += body;

	// convert from UNICODE to ASCII
	l = strBuf.GetLength();
	for(i=0; i<l; i++)
		outBuf[i] = (char)(strBuf.GetAt(i) & 0xFF);
	
	n = 1;
	while(n < nr+1)
	{
		i = m_pCSock->SendTo(outBuf, l, m_nSipSrvPort, (LPCTSTR)m_strSipSrvAddress);
		if(i == SOCKET_ERROR || i != l)
			return -1;
		Sleep(RET_MILISEC);
		i = m_pCSock->ReceiveFrom((void*)pBuf, 1023, servAddress, servPort);
		if(i > 0)
		{
			pBuf[i] = 0;
			if(!strncmp(pBuf, "SIP/2.0 ", 8))
			{
				if(*(pBuf+8) == '2')
				{
					AddSentMessage(to, body);
					UpdateDisplayedMessage();
					return 0;
				}
				if(*(pBuf+8) == '3' || *(pBuf+8) == '4' 
						|| *(pBuf+8) == '5' || *(pBuf+8) == '6')
					return -1;
			}
		}
		n++;
	}
	return -1;
}

/**
 * get local IP address
 */
CString CPocketSMDlg::GetIP()
{
	CString strIp;

	//Init winsock
	WSADATA wsaData;
	int nErrorCode = WSAStartup(MAKEWORD(1,1), &wsaData);
	if (nErrorCode != 0) {
		//Cannot initialize winsock
		return _T("");
	}

	char strHostName[81];
	if (gethostname(strHostName, 80)==0)
	{
		hostent *pHost = gethostbyname(strHostName);
		if (pHost->h_addrtype == AF_INET)
		{
			in_addr **ppip=(in_addr**)pHost->h_addr_list; 
			
			//Enumarate all addresses
			while (*ppip) 
			{
				in_addr ip=**ppip;
				strIp = CString(inet_ntoa(ip));
				ppip++; 
				if (strIp!=_T("")) {
					break;
				}
			}
		}
	}

	return strIp;
}


/**
 * timer handler
 */
void CPocketSMDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CString servAddress;
	int nrBytes=1, nBufLen = 2047;
	UINT servPort;
	char pBuf[2048], oBuf[1024], c, c1;
	str from, body, res;
	CTime crtTime;

	if(nIDEvent == ID_TIMER_A && m_bSetupOK)
	{
		m_nCounter++;
		if(m_nCounter > 3000)
		{
			m_nCounter = 0;
			if(SendSIPRegister("3600", 10) != 0)
			{
				AddInfoMessage("Can not register again. Please restart\r\n");
				UpdateDisplayedMessage();
			}
		}

		if(m_nFlag == 0)
		{
			m_nFlag = 1;
			servAddress.Format(_T("<%s>\r\n  registering to: %s\r\n"),
				m_strUserName, m_strSipSrvAddress);
			AddInfoMessage(servAddress);
			UpdateDisplayedMessage();
			if(SendSIPRegister("3600", 10) != 0)
				AddToLastMessage("Not registered. Please restart\r\n");
			else
				AddToLastMessage("Registered.\r\n");
			UpdateDisplayedMessage();
		}

		res.s = oBuf;
		res.len = 1024;
		// check for MESSAGE messages
		nrBytes = m_pCSock->ReceiveFrom((void*)pBuf, nBufLen, servAddress, servPort);
		while(nrBytes > 7 && (strncmp(pBuf, "MESSAGE", 7) == 0))
		{
			crtTime = CTime::GetCurrentTime();
			pBuf[nrBytes] = 0;
			if(!m_sipMsg.isIdentic(pBuf, nrBytes))
			{
				m_sipMsg.setSIPMsg(pBuf, nrBytes);
				if(!m_bTrayIcon && m_bDlgHide)
				{
					m_bTrayIcon = true;
					SetNewTrayIcon();
				}
				if(m_sipMsg.getFrom(&from) == 0 && m_sipMsg.getBody(&body) == 0)
				{
					// build response
					m_sipMsg.buildResponse(&res, "SIP/2.0 200 OK\r\n", m_strIPAddress, m_nIPPort);
					// send response
					m_pCSock->SendTo(res.s, res.len, m_nSipSrvPort, (LPCTSTR)m_strSipSrvAddress);

					// display to user
					c = from.s[from.len];
					from.s[from.len] = 0;
					c1 = body.s[body.len];
					body.s[body.len] = 0;
					AddRecvMessage(from.s, body.s, crtTime);
					from.s[from.len] = c;
					body.s[body.len] = c1;
					
					UpdateDisplayedMessage();
					PlaySound(TEXT("DOORBELL"), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
					//PlaySound (TEXT("bell.wav"), NULL, SND_SYNC);

				}
				else
				{
					// build response
					m_sipMsg.buildResponse(&res, "SIP/2.0 400 Bad request\r\n", m_strIPAddress, m_nIPPort);
					// send response
					m_pCSock->SendTo(res.s, res.len, m_nSipSrvPort, (LPCTSTR)m_strSipSrvAddress);
					
					// display to user
					AddInfoMessage("Wrong formatted MESSAGE received");
					UpdateDisplayedMessage();
				}

			}
			/***
			else
			{
				m_strEditView = "Non MESSAGE received";
				UpdateData(FALSE);
			}
			***/
			nrBytes = m_pCSock->ReceiveFrom((void*)pBuf, nBufLen, servAddress, servPort);
		}
	}
	else
		if(!m_bSetupOK)
		{
			AddInfoMessage("Config params are not set!\r\nClick on 'Setup' to set them");
			UpdateDisplayedMessage();
		}
	CDialog::OnTimer(nIDEvent);
}

/**
 * PAINT event - callback function
 */
void CPocketSMDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CBitmap bmp, *poldbmp;
	CDC memdc;

	// Load the bitmap resource
	bmp.LoadBitmap( IDB_BITMAP_IPTEL );

	// Create a compatible memory DC
	memdc.CreateCompatibleDC( &dc );

	// Select the bitmap into the DC
	poldbmp = memdc.SelectObject( &bmp );

	// Copy (BitBlt) bitmap from memory DC to screen DC
	dc.BitBlt( 0, 0, 240, 300, &memdc, 0, 0, SRCCOPY );

	memdc.SelectObject( poldbmp );

	// Do not call CDialog::OnPaint() for painting messages
}

/**
 *
 */
void CPocketSMDlg::OnOK() 
{
	FILE *f = NULL;
	char userName[70], srvAddress[50];
	int i, j;
	CString lStr;

	// TODO: Add extra validation here
	SendSIPRegister("0", 2);

	// remove icon from system tray
	RemoveSystemTrayIcon();

	// close the SOCKET
	ASSERT( m_pCSock == NULL );
	m_pCSock->Close();
	
	if((f = fopen("psm.cfg", "wt")) != NULL)
	{
		for(i=0; i < m_strUserName.GetLength(); i++)
			userName[i] = (char)(m_strUserName.GetAt(i) & 0xFF);
		userName[i] = 0;
		for(i=0; i < m_strSipSrvAddress.GetLength(); i++)
			srvAddress[i] = (char)(m_strSipSrvAddress.GetAt(i) & 0xFF);
		srvAddress[i] = 0;
		fprintf(f, "%s %s %d\r\n", userName, 
			srvAddress, m_nSipSrvPort);

		int nCount = m_ctlComboTo.GetCount();
		for(j=0; nCount!= CB_ERR && j<nCount; j++)
		{
			m_ctlComboTo.GetLBText(j, lStr);
			for(i=0; lStr.GetLength()>0 && i<lStr.GetLength(); i++)
				userName[i] = (char)(lStr.GetAt(i) & 0xFF);
			userName[i] = 0;
			fprintf(f, "%s\r\n", userName); 
		}
		fclose(f);
	}

	CDialog::OnOK();
}

/**
 * display SETUP dialog
 */
void CPocketSMDlg::OnButtonSetup() 
{
	// TODO: Add your control notification handler code here
	// m_TrayIcon.MinimiseToTray(this);
	if(m_dlgSetup->DoModal() == IDOK)
	{
		if(!m_bSetupOK)
			m_bSetupOK = true;
		if(m_dlgSetup->getUserName().GetLength() > 0)
			m_strUserName = m_dlgSetup->getUserName();
		if(m_dlgSetup->getServerAddress().GetLength() > 0)
			m_strSipSrvAddress = m_dlgSetup->getServerAddress();
		if(m_dlgSetup->getServerPort()  > 0)
			m_nSipSrvPort = m_dlgSetup->getServerPort();
		m_nFlag = 0;
	}
}

/**
 * hide the application's window
 */
void CPocketSMDlg::OnButtonHide() 
{
	// TODO: Add your control notification handler code here
	// m_TrayIcon.MinimiseToTray(this);
	if(!m_bDlgHide)
	{
		ShowWindow(SW_HIDE);
		m_bDlgHide = true;
	}
}

/**
 * add the icon in system tray
 */
void CPocketSMDlg::AddSystemTrayIcon() 
{
 // handle to icon
     HICON hIcon;

     HINSTANCE hInst =
         AfxFindResourceHandle(MAKEINTRESOURCE(IDR_MAINFRAME),
                               RT_GROUP_ICON);
         
      hIcon = (HICON)LoadImage( hInst,
                               MAKEINTRESOURCE(IDR_MAINFRAME),
                               IMAGE_ICON,
                               16,
                               16,
                               LR_DEFAULTCOLOR);
   // set NOTIFYCONDATA structure
     m_nidIconData.cbSize = sizeof(NOTIFYICONDATA);
     m_nidIconData.hWnd = m_hWnd;
     m_nidIconData.uID = IDR_MAINFRAME;
     m_nidIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
     m_nidIconData.uCallbackMessage = WM_ICON_NOTIFY; // my user message
     m_nidIconData.hIcon = hIcon;

     m_nidIconData.szTip[0] = '\0';

  // call to Shell_NotifyIcon with NIM_ADD parameter

     Shell_NotifyIcon(NIM_ADD, &m_nidIconData);
 
  // free icon 
     if (hIcon) 
		DestroyIcon(hIcon); 

}

/**
 * remove the icon from system tray
 */
void CPocketSMDlg::RemoveSystemTrayIcon() 
{

  // call to Shell_NotifyIcon with NIM_DEL parameter

    Shell_NotifyIcon(NIM_DELETE, &m_nidIconData); 
}

/**
 * set a new icon in system tray
 */
void CPocketSMDlg::SetNewTrayIcon()
{
	RemoveSystemTrayIcon();
 // handle to icon
     HICON hIcon;

     HINSTANCE hInst =
         AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_NEW),
                               RT_GROUP_ICON);
         
      hIcon = (HICON)LoadImage( hInst,
                               MAKEINTRESOURCE(IDI_ICON_NEW),
                               IMAGE_ICON,
                               16,
                               16,
                               LR_DEFAULTCOLOR);
   // set NOTIFYCONDATA structure
     m_nidIconData.cbSize = sizeof(NOTIFYICONDATA);
     m_nidIconData.hWnd = m_hWnd;
     m_nidIconData.uID = IDI_ICON_NEW;
     m_nidIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
     m_nidIconData.uCallbackMessage = WM_ICON_NOTIFY; // my user message
     m_nidIconData.hIcon = hIcon;

     m_nidIconData.szTip[0] = '\0';

  // call to Shell_NotifyIcon with NIM_ADD parameter

     Shell_NotifyIcon(NIM_ADD, &m_nidIconData);
 
  // free icon 
     if (hIcon) 
		DestroyIcon(hIcon); 
}

/**
 * set the old icon in system tray
 */
void CPocketSMDlg::SetOldTrayIcon() 
{
	RemoveSystemTrayIcon();
	// handle to icon
     HICON hIcon;

     HINSTANCE hInst =
         AfxFindResourceHandle(MAKEINTRESOURCE(IDR_MAINFRAME),
                               RT_GROUP_ICON);
         
      hIcon = (HICON)LoadImage( hInst,
                               MAKEINTRESOURCE(IDR_MAINFRAME),
                               IMAGE_ICON,
                               16,
                               16,
                               LR_DEFAULTCOLOR);
   // set NOTIFYCONDATA structure
     m_nidIconData.cbSize = sizeof(NOTIFYICONDATA);
     m_nidIconData.hWnd = m_hWnd;
     m_nidIconData.uID = IDR_MAINFRAME;
     m_nidIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
     m_nidIconData.uCallbackMessage = WM_ICON_NOTIFY; // my user message
     m_nidIconData.hIcon = hIcon;

     m_nidIconData.szTip[0] = '\0';

  // call to Shell_NotifyIcon with NIM_ADD parameter

     Shell_NotifyIcon(NIM_ADD, &m_nidIconData);
 
  // free icon 
     if (hIcon) 
		DestroyIcon(hIcon); 

}

/**
 * system tray icon notification handler
 */
LRESULT	CPocketSMDlg::OnIconNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uMouseMsg = (UINT) lParam;

	switch (uMouseMsg)
	{
		case WM_LBUTTONDOWN: 
			if(m_bDlgHide)
			{
				m_bDlgHide = false;
				ShowWindow(SW_SHOW);
				SetActiveWindow();
			}
			SetForegroundWindow();
			if(m_bTrayIcon)
			{
				m_bTrayIcon = false;
				SetOldTrayIcon();
			}
			//ShowTrayMenu();
			break;

		default: break;
	}
		
	return 0;
}


void CPocketSMDlg::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctlComboTo.GetCurSel();	
	CString lStr, lStr1;

	m_ctlComboTo.GetWindowText(lStr);
	lStr.TrimLeft();
	lStr.TrimRight();
	if(nIndex == CB_ERR)
	{
		if(lStr.GetLength() > 3 && lStr.Find('@')>0)
		{
			if(m_ctlComboTo.FindStringExact(0, (LPCTSTR)lStr) == CB_ERR)
			{
				nIndex = m_ctlComboTo.GetCount();
				if(nIndex != CB_ERR && nIndex >= MAX_CONTACTS)
					m_ctlComboTo.DeleteString(nIndex-1);
				m_ctlComboTo.InsertString(0, lStr);
			}
		}
		else
		{
			lStr1.Format(_T("\"%s\" might be invalid SIP address!"), lStr);
			AfxMessageBox(lStr1);
			return;
		}

	}
	else
	{
		if(nIndex != 0)
		{
			m_ctlComboTo.DeleteString(nIndex);
			m_ctlComboTo.InsertString(0, lStr);
			m_ctlComboTo.SetCurSel(0);
		}
	}
	m_ctlEditMsg.GetWindowText(m_strEditMsg);
	m_strEditMsg.TrimLeft();
	m_strEditMsg.TrimRight();
	if(m_strEditMsg.GetLength() <= 0)
	{
		AfxMessageBox(_T("No message to send!"));
		return;
	}
	if(SendSIPMessage(lStr, m_strEditMsg, 5) != 0)
	{
		AfxMessageBox(_T("The message could not be sent!"));
		return;
	}
	else
	{
		m_strEditMsg = "";
		m_ctlEditMsg.SetWindowText(m_strEditMsg);
	}
}

void CPocketSMDlg::AddInfoMessage(CString s)
{
	msgList[last] = ".\r\n";
	msgList[last] += s;
	last = (last+1)%MSG_LIST_SIZE;
	if(size < MSG_LIST_SIZE)
		size++;
	else
		first = (first+1)%MSG_LIST_SIZE;
}
void CPocketSMDlg::AddSentMessage(CString t, CString s)
{
	msgList[last].Format(_T(".To: <%s>\r\n%s\r\n"),
		t, s);
	last = (last+1)%MSG_LIST_SIZE;
	if(size < MSG_LIST_SIZE)
		size++;
	else
		first = (first+1)%MSG_LIST_SIZE;
}
void CPocketSMDlg::AddRecvMessage(CString f, CString s, CTime t)
{
	msgList[last].Format(_T(".From: %s\r\n  [%d-%02d-%02d %02d:%02d:%02d]\r\n%s\r\n"), 
		f, t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond(), s);
	last = (last+1)%MSG_LIST_SIZE;
	if(size < MSG_LIST_SIZE)
		size++;
	else
		first = (first+1)%MSG_LIST_SIZE;
}
void CPocketSMDlg::AddToLastMessage(CString s)
{
	if(size == 0)
	{
		AddInfoMessage(s);
		return;
	}
	msgList[(last+MSG_LIST_SIZE-1)%MSG_LIST_SIZE] += s;
}

void CPocketSMDlg::UpdateDisplayedMessage()
{
	int n,i;
	if(size == 0)
		return;
	n = size;
	i = last-1;
	m_strEditView="";
	while(n>0)
	{
		m_strEditView += msgList[i] + "\r\n";
		i = (i+MSG_LIST_SIZE-1)%MSG_LIST_SIZE;
		n--;
	}
	UpdateData(FALSE);
}

void CPocketSMDlg::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

/***
void CPocketSMDlg::OnButtonDeleteContact() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctlComboTo.GetCurSel();	
	if(nIndex == CB_ERR)
	{
		AfxMessageBox(_T("No contact selected from the list."));
		return;
	}

	if(m_ctlComboTo.DeleteString(nIndex) == CB_ERR)
	{
		AfxMessageBox(_T("The contact could not be deleted."));
		return;
	}
	m_ctlComboTo.SetCurSel(0);
	
}
***/
