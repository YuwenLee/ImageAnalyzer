#pragma once


// CRawFormatDlg 對話方塊

class CRawFormatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRawFormatDlg)

public:
	CRawFormatDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CRawFormatDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RAW_FORMAT };
#endif

protected:
	int m_nFormat;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int GetFormat();
	afx_msg void OnCbnSelchangeFormat();
};
