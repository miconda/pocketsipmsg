// SetupDlg.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "SetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog


CSetupDlg::CSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDlg)
	m_strSrvAddress = _T("");
	m_nSrvPort = 0;
	m_strUserName = _T("");
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
}


void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDlg)
	DDX_Text(pDX, IDC_EDIT_SRVADDRESS, m_strSrvAddress);
	DDX_Text(pDX, IDC_EDIT_SRVPORT, m_nSrvPort);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg message handlers

void CSetupDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	
	CDialog::OnOK();
}

CString CSetupDlg::getUserName()
{
	return m_strUserName;
}
CString CSetupDlg::getServerAddress()
{
	return m_strSrvAddress;
}
CString CSetupDlg::getPassword()
{
	return m_strPassword;
}
UINT CSetupDlg::getServerPort()
{
	return m_nSrvPort;
}

void CSetupDlg::setUserName(CString param)
{
	m_strUserName = param;
}
void CSetupDlg::setServerAddress(CString param)
{
	 m_strSrvAddress = param;
}
void CSetupDlg::setPassword(CString param)
{
	m_strPassword = param;
}
void CSetupDlg::setServerPort(UINT param)
{
	 m_nSrvPort = param;
}
