// PocketSM.h : main header file for the PocketSM application
//

#if !defined(AFX_POCKETSM_H__49A6386E_8308_460E_8858_10D541D631AB__INCLUDED_)
#define AFX_POCKETSM_H__49A6386E_8308_460E_8858_10D541D631AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPocketSMApp:
// See PocketSM.cpp for the implementation of this class
//

class CPocketSMApp : public CWinApp
{
public:
	CPocketSMApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPocketSMApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPocketSMApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POCKETSM_H__49A6386E_8308_460E_8858_10D541D631AB__INCLUDED_)
