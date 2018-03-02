
// ImageAnalyzerView.cpp : CImageAnalyzerView 類別的實作
//

#include "stdafx.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "ImageAnalyzer.h"
#endif

#include "ImageAnalyzerDoc.h"
#include "ImageAnalyzerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageAnalyzerView

IMPLEMENT_DYNCREATE(CImageAnalyzerView, CScrollView)

BEGIN_MESSAGE_MAP(CImageAnalyzerView, CScrollView)
	ON_COMMAND(ID_FILE_OPEN, &CImageAnalyzerView::OnFileOpen)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CImageAnalyzerView 建構/解構

CImageAnalyzerView::CImageAnalyzerView()
	: m_strBMPFileName(_T(""))
	, m_hBMP(NULL)
	, m_nBMPWidth(0)
	, m_nBMPHeight(0)
{
	// TODO: 在此加入建構程式碼

}

CImageAnalyzerView::~CImageAnalyzerView()
{
}

BOOL CImageAnalyzerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的

	return CScrollView::PreCreateWindow(cs);
}

// CImageAnalyzerView 描繪

void CImageAnalyzerView::OnDraw(CDC* pDC)
{
	CImageAnalyzerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此加入原生資料的描繪程式碼
	CClientDC      clientDC(this);
	CDC            mDC;
	CRect          rect;
	int            rgbLine = m_bmp.GetWidth() * 3;
	unsigned char *pRGB = m_bmp.GetRGB();
	int            client_w, client_h, i, j;
	CBitmap       *p_bmp;

	GetClientRect(&rect);
	client_w = rect.Width();
	client_h = rect.Height();

	mDC.CreateCompatibleDC(&clientDC);
	p_bmp = mDC.SelectObject(&m_bmpGDI);
	clientDC.BitBlt(0, 0, rect.right, rect.bottom, &mDC, 0, 0, SRCCOPY);
	clientDC.SelectObject(p_bmp);

	/*
	for (j = 0; j < m_nBMPHeight; j++)
	{
		unsigned char *ptr;
		ptr = pRGB + rgbLine*j;
		COLORREF color_ref;

		for (i = 0; i < m_nBMPWidth; i++)
        {
			color_ref = *(ptr + i*3)<<16 | *(ptr + i*3 + 1) << 8 | *(ptr + i*3 + 2);
			pDC->SetPixel(i, j, COLORREF(color_ref));
			if (i > client_w) break;
		}
		if (j > client_h) break;
	}
	*/
}

void CImageAnalyzerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 計算此檢視的總大小
	sizeTotal.cx = 100;
	sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CImageAnalyzerView 診斷

#ifdef _DEBUG
void CImageAnalyzerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CImageAnalyzerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CImageAnalyzerDoc* CImageAnalyzerView::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageAnalyzerDoc)));
	return (CImageAnalyzerDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageAnalyzerView 訊息處理常式


void CImageAnalyzerView::OnFileOpen()
{
	// TODO: 在此加入您的命令處理常式程式碼
	CFileDialog dlg(TRUE, _T("Pictures"), _T("*.bmp"));
	BOOL        bOK;
	CSize       srcollSize;

	if (m_FileBMP.m_hFile != CFile::hFileNull) {
	    m_FileBMP.Close();
    }

	if (IDOK == dlg.DoModal()) {
		m_strBMPFileName = dlg.GetPathName();
		bOK = m_FileBMP.Open(m_strBMPFileName, CFile::modeRead | CFile::typeBinary);

		if (!bOK) {
			return;
		}

		//
		// Load the Image File
		//
		unsigned int uSize;

		uSize = (unsigned int)m_FileBMP.GetLength();
		m_bmp.SetBufferSize(uSize);
		m_FileBMP.Read(m_bmp.GetFileHeader(), uSize);
		m_FileBMP.Close();

		m_hBMP = (HBITMAP)::LoadImage(
			GetModuleHandle(NULL),
			m_strBMPFileName,
			IMAGE_BITMAP, 0, 0,
			LR_DEFAULTSIZE | LR_LOADFROMFILE);
		m_bmpGDI.Detach();
		m_bmpGDI.Attach(m_hBMP);

		//
		// Set the size of scroll view
		//
		m_nBMPWidth = srcollSize.cx = m_bmp.GetWidth();
		m_nBMPHeight = srcollSize.cy = m_bmp.GetHeight();
            
		SetScrollSizes(MM_TEXT, srcollSize);


	}
}


int CImageAnalyzerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此加入特別建立的程式碼

	return 0;
}


void CImageAnalyzerView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此加入您的訊息處理常式程式碼
					   // 不要呼叫圖片訊息的 CScrollView::OnPaint()

	CDC            mDC;
	CRect          rect;
	int            rgbLine = m_bmp.GetWidth() * 3;
	unsigned char *pRGB = m_bmp.GetRGB();
	int            client_w, client_h, i, j;
	CBitmap       *p_bmp;

	GetClientRect(&rect);
	client_w = rect.Width();
	client_h = rect.Height();

	mDC.CreateCompatibleDC(&dc);
	p_bmp = mDC.SelectObject(&m_bmpGDI);
	dc.BitBlt(0, 0, rect.right, rect.bottom, &mDC, 0, 0, SRCCOPY);
	dc.SelectObject(p_bmp);

	
	for (j = 0; j < m_nBMPHeight; j++)
	{
		unsigned char *ptr;
		ptr = pRGB + rgbLine*j;
		COLORREF color_ref;

		for (i = 0; i < m_nBMPWidth; i++)
		{
			color_ref = *(ptr + i*3)<<16 | *(ptr + i*3 + 1) << 8 | *(ptr + i*3 + 2);
			//dc.SetPixel(i, j, COLORREF(color_ref));
			if (i > client_w) break;
		}
		if (j > client_h) break;
	}
	
}
