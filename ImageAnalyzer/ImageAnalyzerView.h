
// ImageAnalyzerView.h : CImageAnalyzerView ���O������
//

#pragma once
#include "afx.h"
#include "CBMP.h"


class CImageAnalyzerView : public CScrollView
{
protected: // �ȱq�ǦC�ƫإ�
	CImageAnalyzerView();
	DECLARE_DYNCREATE(CImageAnalyzerView)

	// �ݩ�
public:
	CImageAnalyzerDoc* GetDocument() const;

	// �@�~
public:

	// �мg
public:
	virtual void OnDraw(CDC* pDC);  // �мg�H�yø���˵�
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // �غc��Ĥ@���I�s

// �{���X��@
public:
	virtual ~CImageAnalyzerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ͪ��T�������禡
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
	int m_nViewWidth;
	int m_nViewHeight;
	int m_nMeasureX;
	int m_nMeasureY;
	int m_nAVRGr;
	int m_nAVRGg;
	int m_nAVRGb;

	CPoint m_MeasurePoint1, m_MeasurePoint2;
	int    m_MeasureCnt;
	int    m_R[6];
	int    m_G[6];
	int    m_B[6];

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // ImageAnalyzerView.cpp ������������
inline CImageAnalyzerDoc* CImageAnalyzerView::GetDocument() const
   { return reinterpret_cast<CImageAnalyzerDoc*>(m_pDocument); }
#endif
