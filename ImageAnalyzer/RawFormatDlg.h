#pragma once


// CRawFormatDlg ��ܤ��

class CRawFormatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRawFormatDlg)

public:
	CRawFormatDlg(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CRawFormatDlg();

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RAW_FORMAT };
#endif

protected:
	int m_nFormat;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int GetFormat();
	afx_msg void OnCbnSelchangeFormat();
};
