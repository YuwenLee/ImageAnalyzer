// RawFormatDlg.cpp : ��@��
//

#include "stdafx.h"
#include "ImageAnalyzer.h"
#include "RawFormatDlg.h"
#include "afxdialogex.h"


// CRawFormatDlg ��ܤ��

IMPLEMENT_DYNAMIC(CRawFormatDlg, CDialogEx)

CRawFormatDlg::CRawFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RAW_FORMAT, pParent)
{
	m_nWidth = 4032;
	m_nHeight = 3024;
	m_nFormat = 0;
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


// CRawFormatDlg �T���B�z�`��


void CRawFormatDlg::OnBnClickedOk()
{
	// TODO: �b���[�J����i���B�z�`���{���X
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

	// TODO:  �b���[�J�B�~����l��
	CComboBox *pBox;
	CEdit     *pEdit;
	CString    str;

	pBox = (CComboBox *)GetDlgItem(IDC_FORMAT);
	pBox->SetCurSel(m_nFormat);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_WIDTH);
	str.Format(L"%d", m_nWidth);
	pEdit->SetWindowText(str);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_HEIGHT);
	str.Format(L"%d", m_nHeight);
	pEdit->SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX �ݩʭ����Ǧ^ FALSE
}

int CRawFormatDlg::GetFormat()
{
	return m_nFormat;
}


void CRawFormatDlg::OnCbnSelchangeFormat()
{
	// TODO: �b���[�J����i���B�z�`���{���X
}


int CRawFormatDlg::GetWidthHeight(int * pnWidth, int * pnHeight)
{
	*pnWidth = m_nWidth;
	*pnHeight = m_nHeight;

	return 0;
}

void CRawFormatDlg::setInitState(int nWidth, int nHeight, int nFormat)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nFormat = nFormat;

	return;
}
