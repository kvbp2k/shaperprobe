
// shapertoolDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CshapertoolDlg dialog
class CshapertoolDlg : public CDialog
{
// Construction
public:
	CshapertoolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SHAPERTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CButton m_cOK;
	CEdit m_cEdit;
	CProgressCtrl m_nProgress;
	CStatic m_sStatus;
};
