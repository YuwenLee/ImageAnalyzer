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

	pBox = (CComboBox *)GetDlgItem(IDC_FORMAT);
	m_nFormat = pBox->GetCurSel();

	CDialogEx::OnOK();
}

BOOL CRawFormatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �b���[�J�B�~����l��
	CComboBox *pBox;

	pBox = (CComboBox *)GetDlgItem(IDC_FORMAT);
	pBox->SetCurSel(0);
	m_nFormat = 1;

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
