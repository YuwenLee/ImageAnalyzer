
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
	CString m_strInputFileName;
	CString m_strBMPFileName;
	CFile   m_FileBMP;
	HBITMAP m_hBMP;
	CBitmap m_bmpGDI; // GID Object
	BMP     m_bmp;

	CString m_strResultFile;

private:
	int  m_nBMPWidth;  // For Drawing
	int  m_nBMPHeight; // For Drawing
	int  m_nViewWidth;
	int  m_nViewHeight;
	int  m_nMeasureX;
	int  m_nMeasureY;
	int  m_nAVRGr;
	int  m_nAVRGg;
	int  m_nAVRGb;
	BOOL m_bGotButtonDown;

	// EXIF
	int    m_nExposure; // time = 1/m_nExposure
	int    m_nISO;
	int    m_nFnum_n;   // F# = n.f
	int    m_nFnum_f;   // F# = n.f
	int    m_nYear;
	int    m_nMonth;
	int    m_nDay;
	int    m_nHour;
	int    m_nMin;
	int    m_nSec;
	// Measure R/G and B/G ratios
	CPoint m_MeasurePoint1, m_MeasurePoint2;
	int    m_MeasureCnt;
	int    m_R[6];
	int    m_G[6];
	int    m_B[6];
	CRect  m_RectMeasureRGB[6];

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	void SaveResult(CString strFileName);
	CString GenerateBMP(CString strFileName);
	CString getValue(CString strFilename, CString strTag);
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	int DrawRect(CDC *pDC, CPoint p1, CPoint p2);
};

#ifndef _DEBUG  // ImageAnalyzerView.cpp 中的偵錯版本
inline CImageAnalyzerDoc* CImageAnalyzerView::GetDocument() const
   { return reinterpret_cast<CImageAnalyzerDoc*>(m_pDocument); }
#endif

