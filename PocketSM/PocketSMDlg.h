// PocketSMDlg.h : header file
//

#if !defined(AFX_POCKETSMDLG_H__A14A2724_D2B5_4C90_898A_8F8EBA0C1241__INCLUDED_)
#define AFX_POCKETSMDLG_H__A14A2724_D2B5_4C90_898A_8F8EBA0C1241__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SIPMsg.h"
#include "SetupDlg.h"

#define WM_ICON_NOTIFY  WM_APP+230


/////////////////////////////////////////////////////////////////////////////
// CPocketSMDlg dialog

class CPocketSMDlg : public CDialog
{
// Construction
public:
	CPocketSMDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPocketSMDlg)
	enum { IDD = IDD_DIALOG_MAIN };
	CButton	m_ctlButtonSend;
	CEdit	m_ctlEditMsg;
	CComboBox	m_ctlComboTo;
	CString	m_strEditView;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPocketSMDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	
	CString m_strEditMsg;

	CAsyncSocket	*m_pCSock;	// communication socket
	UINT	m_nIPPort;			// local port number
	CString m_strIPAddress;		// local address
	CString m_strUserName;		// user name
	CString m_strUserPart;		// user part of username
	CString m_strSrvPart;		// server part of username
	CString m_strPassword;		// user password
	CString m_strAuthHdr;		// realm
	CString m_strSipSrvAddress;	// server address
	UINT	m_nSipSrvPort;		// server port number
	int		m_nFlag;			// flag
	int		m_nCounter;			// message couter
	int		m_nCallID;			// Call ID couter
	CSIPMsg m_sipMsg;			// sip message object
	bool	m_bDlgHide;			// the dialog is visible?
	bool	m_bTrayIcon;		// tray icon state
	bool	m_bSetupOK;			// setup is ok?
	CSetupDlg* m_dlgSetup;		// setup dialog object

	int size;
	int first;
	int last;
	int MSG_LIST_SIZE; // !!! change in class declaration too (msgList)
	CString msgList[10];

	NOTIFYICONDATA	m_nidIconData;

	CString GetIP();
	void AddSystemTrayIcon();
	void SetNewTrayIcon();
	void SetOldTrayIcon();
	void RemoveSystemTrayIcon();
	int SendSIPRegister(char* expire, int nr);
	int SendSIPMessage(CString to, CString body, int nr);
	
	int computeAuthHeader(CString sipRepl);
	int translateToStr(CString s, str *ps);

	void AddInfoMessage(CString s);
	void AddToLastMessage(CString s);
	void AddSentMessage(CString t, CString s);
	void AddRecvMessage(CString f, CString s, CTime t);
	void UpdateDisplayedMessage();
	void UpdateCallID();
	// Generated message map functions
	//{{AFX_MSG(CPocketSMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	virtual void OnOK();
	afx_msg void OnButtonSetup();
	afx_msg void OnButtonHide();
	afx_msg void OnButtonSend();
	afx_msg void OnButtonExit();
	//}}AFX_MSG
	LRESULT	OnIconNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POCKETSMDLG_H__A14A2724_D2B5_4C90_898A_8F8EBA0C1241__INCLUDED_)
