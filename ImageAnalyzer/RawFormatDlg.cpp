// RawFormatDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "ImageAnalyzer.h"
#include "RawFormatDlg.h"
#include "afxdialogex.h"


// CRawFormatDlg 對話方塊

IMPLEMENT_DYNAMIC(CRawFormatDlg, CDialogEx)

CRawFormatDlg::CRawFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RAW_FORMAT, pParent)
{

}

CRawFormatDlg::~CRawFormatDlg()
{
}

void CRawFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRawFormatDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRawFormatDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_FORMAT, &CRawFormatDlg::OnCbnSelchangeFormat)
END_MESSAGE_MAP()


// CRawFormatDlg 訊息處理常式


void CRawFormatDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CComboBox *pBox;
	CEdit     *pEdit;
	CString    str;

	pBox = (CComboBox *)GetDlgItem(IDC_FORMAT);
	m_nFormat = pBox->GetCurSel();

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_WIDTH);
	pEdit->GetWindowText(str);
	m_nWidth = _ttoi(str);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_HEIGHT);
	pEdit->GetWindowText(str);
	m_nHeight = _ttoi(str);


	CDialogEx::OnOK();
}

BOOL CRawFormatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CComboBox *pBox;

	pBox = (CComboBox *)GetDlgItem(IDC_FORMAT);
	pBox->SetCurSel(0);
	m_nFormat = 1;
	m_nWidth = 0;
	m_nHeight = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

int CRawFormatDlg::GetFormat()
{
	return m_nFormat;
}


void CRawFormatDlg::OnCbnSelchangeFormat()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}


int CRawFormatDlg::GetWidthHeight(int * pnWidth, int * pnHeight)
{
	*pnWidth = m_nWidth;
	*pnHeight = m_nHeight;

	return 0;
}
