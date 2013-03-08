
// shapertoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shapertool.h"
#include "shapertoolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CshapertoolDlg dialog




CshapertoolDlg::CshapertoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CshapertoolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CshapertoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_cOK);
	DDX_Control(pDX, IDC_EDIT2, m_cEdit);
	DDX_Control(pDX, IDC_PROGRESS, m_nProgress);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_sStatus);
}

BEGIN_MESSAGE_MAP(CshapertoolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CshapertoolDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CshapertoolDlg message handlers

BOOL CshapertoolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_cEdit.SetWindowText("Click \"OK\" to start new measurement.\r\n");
	m_nProgress.SetRange(0, 100);
	m_nProgress.SetPos(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CshapertoolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CshapertoolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

extern "C" int main123(void *edit, int (*eprintf)(char *,void  *), int (*eprogress)(int,void *), int (*estatus)(char *str, void *t));

int eprintf(char *str, void *t)
{
	CString text;
	((CshapertoolDlg *)t)->m_cEdit.GetWindowTextA(text);
	((CshapertoolDlg *)t)->m_cEdit.SetWindowTextA(text+str);

	return 0;
}

int eprogress(int pos, void *t)
{
	((CshapertoolDlg *)t)->m_nProgress.SetPos(pos);
	return 0;
}

int estatus(char *str, void *t)
{
	((CshapertoolDlg *)t)->m_sStatus.SetWindowTextA(str);
	return 0;
}

void thr(void *t)
{
	((CshapertoolDlg *)t)->m_cEdit.SetWindowTextA("");
	((CshapertoolDlg *)t)->m_sStatus.SetWindowTextA("");
	((CshapertoolDlg *)t)->m_nProgress.SetPos(0);
	if(main123(t, eprintf, eprogress, estatus) == -1)
	{
		((CshapertoolDlg *)t)->m_cEdit.SetWindowTextA("Server busy or measurement aborted. Please try again later.\r\n");
	}
	((CshapertoolDlg *)t)->m_cOK.SetWindowText("Restart");
	((CshapertoolDlg *)t)->m_cOK.EnableWindow(1);
}

void CshapertoolDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString text; ((CshapertoolDlg *)this)->m_cEdit.GetWindowTextA(text);

	m_cOK.EnableWindow(FALSE);
	HANDLE hThread = (HANDLE)_beginthread(thr, 0, (void *)this);
	//WaitForSingleObject(hThread, INFINITE);

	//OnOK();
}
