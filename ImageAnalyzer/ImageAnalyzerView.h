
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
	int     m_nAction;
	int     m_nZoomPercent;
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
	float  m_fBV;       // Brightness Value
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
	float  m_L[6];
	CRect  m_RectMeasureRGB[6];

	//
	// RAW Data Format
	//
	int    m_nRAWDlg_width;
	int    m_nRAWDlg_height;
	int    m_nRAWDlg_format;

	//
	// Calibration Data
	//
	int     m_nCalibrationCnt;     // max value 20
	int     m_nVertex;             // max value 4
	CString m_strILL_Name[20];     // file names
	CPoint  m_MCC_Vertices[20][4]; // MCCs
	int     m_nILL_CTT[20];        // Color Temperatures
	float   m_fduv[20];            // delta UVs
	CPoint  m_ptCal[20][2];
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	void SaveResult(CString strFileName);
	int JPG_to_BMP(CString strFileName, CString strBMPFileName);
	int RAW_to_BMP_Dir(CString strDirName);
	int RAW_to_BMP(CString strFileName, CString strBMPFileName, int nWidth = 0, int nHeight = 0, int nFormat = -1);
	int MeasureRGB(CPoint point);
	int ReMeasureRGB();
	int SaveRGB();
	int MeasureCalibrationData_FileOpen();
	int MeasureCalibrationData(CPoint point);
	int ReMeasureCalibrationData();
	int MeasureWBGain(CPoint point);
	int SaveCalibration();
	int SaveBMPFile();
	int LoadBMPFile(CString strFileName);
	CString getValue(CString strFilename, CString strTag);
	int setValue(CString strFileName, CString strTag, int nValue);
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	int DrawRect(CDC *pDC, CPoint p1, CPoint p2);
	int RecoverRect(CDC *pDC, CPoint p1, CPoint p2);
public:
	afx_msg void OnActionTeacherdata();
	afx_msg void OnUpdateActionTeacherdata(CCmdUI *pCmdUI);
	afx_msg void OnUpdateActionMeasure(CCmdUI *pCmdUI);
	afx_msg void OnActionMeasure();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateActionCalibration(CCmdUI *pCmdUI);
	afx_msg void OnActionCalibration();
	afx_msg void OnFileSaveAs();
	void SaveBmpAsRaw(CString strBMPFileName, CString strRAWFileName);
	int GenerateTeacher(CString strJPGFile);
	afx_msg void OnUpdateActionTransformRaw(CCmdUI *pCmdUI);
	afx_msg void OnActionTransformRaw();
protected:
	int PackedRAW_to_UnpackedRAW_DIR(CString strDirName);
public:
	afx_msg void OnActionBmp();
	afx_msg void OnActionUnpack();
	afx_msg void OnActionEval();
protected:
	int GenerateTeacher_Dir(CString strDirName);
};

#ifndef _DEBUG  // ImageAnalyzerView.cpp 中的偵錯版本
inline CImageAnalyzerDoc* CImageAnalyzerView::GetDocument() const
   { return reinterpret_cast<CImageAnalyzerDoc*>(m_pDocument); }
#endif

