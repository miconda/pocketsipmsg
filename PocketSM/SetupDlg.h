#if !defined(AFX_SETUPDLG_H__F0E1D1A0_8A5D_45B2_A1FB_5BE4FC796F49__INCLUDED_)
#define AFX_SETUPDLG_H__F0E1D1A0_8A5D_45B2_A1FB_5BE4FC796F49__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog

class CSetupDlg : public CDialog
{
// Construction
public:
	CSetupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupDlg)
	enum { IDD = IDD_DIALOG_SETUP };
	CString	m_strSrvAddress;
	UINT	m_nSrvPort;
	CString	m_strUserName;
	CString	m_strPassword;
	CString	m_strMyContact;
	//}}AFX_DATA

	CString getUserName();
	CString getServerAddress();
	CString getPassword();
	UINT getServerPort();

	void setMyContact(CString addr, int port);
	void setUserName(CString param);
	void setServerAddress(CString param);
	void setPassword(CString param);
	void setServerPort(UINT param);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDLG_H__F0E1D1A0_8A5D_45B2_A1FB_5BE4FC796F49__INCLUDED_)
