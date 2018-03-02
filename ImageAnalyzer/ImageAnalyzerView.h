
// ImageAnalyzerView.h : CImageAnalyzerView 類別的介面
//

#pragma once
#include "afx.h"
#include "CBMP.h"


class CImageAnalyzerView : public CScrollView
{
protected: // 僅從序列化建立
	CImageAnalyzerView();
	DECLARE_DYNCREATE(CImageAnalyzerView)

// 屬性
public:
	CImageAnalyzerDoc* GetDocument() const;

// 作業
public:

// 覆寫
public:
	virtual void OnDraw(CDC* pDC);  // 覆寫以描繪此檢視
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 建構後第一次呼叫

// 程式碼實作
public:
	virtual ~CImageAnalyzerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
protected:
	CString m_strBMPFileName;
	CFile   m_FileBMP;
	HBITMAP m_hBMP;
	CBitmap m_bmpGDI; // GID Object
	BMP     m_bmp;
private:
	int m_nBMPWidth;  // For Drawing
	int m_nBMPHeight; // For Drawing
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};

#ifndef _DEBUG  // ImageAnalyzerView.cpp 中的偵錯版本
inline CImageAnalyzerDoc* CImageAnalyzerView::GetDocument() const
   { return reinterpret_cast<CImageAnalyzerDoc*>(m_pDocument); }
#endif

