
// ImageAnalyzerView.cpp : CImageAnalyzerView ���O����@
//

#include "stdafx.h"
// SHARED_HANDLERS �i�H�w�q�b��@�w���B�Y�ϩM�j�M�z�����B�z�`����
// ATL �M�פ��A�ä��\�P�ӱM�צ@�Τ��{���X�C
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
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CImageAnalyzerView �غc/�Ѻc

CImageAnalyzerView::CImageAnalyzerView()
	: m_strBMPFileName(_T(""))
	, m_hBMP(NULL)
	, m_nBMPWidth(0)
	, m_nBMPHeight(0)
	, m_nViewWidth(0)
	, m_nViewHeight(0)
	, m_nMeasureX(0)
	, m_nMeasureY(0)
	, m_nAVRGr(0)
	, m_nAVRGb(0)
	, m_nAVRGg(0)
{
	// TODO: �b���[�J�غc�{���X

}

CImageAnalyzerView::~CImageAnalyzerView()
{
}

BOOL CImageAnalyzerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �b���g�ѭק� CREATESTRUCT cs 
	// �F��ק�������O�μ˦����ت�

	return CScrollView::PreCreateWindow(cs);
}

// CImageAnalyzerView �yø

void CImageAnalyzerView::OnDraw(CDC* pDC)
{
	CImageAnalyzerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �b���[�J��͸�ƪ��yø�{���X
	CClientDC      clientDC(this);
	//int            rgbLine = m_bmp.GetWidth() * 3;
	//unsigned char *pRGB = m_bmp.GetRGB();
	//int            i, j;
	CBitmap       *p_bmp;

	pDC->CreateCompatibleDC(&clientDC);
	p_bmp = pDC->SelectObject(&m_bmpGDI);
	clientDC.BitBlt(0, 0, m_bmp.GetWidth(), m_bmp.GetHeight(), pDC, 0, 0, SRCCOPY);
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
	// TODO: �p�⦹�˵����`�j�p
	sizeTotal.cx = 100;
	sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CImageAnalyzerView �E�_

#ifdef _DEBUG
void CImageAnalyzerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CImageAnalyzerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CImageAnalyzerDoc* CImageAnalyzerView::GetDocument() const // ���O�D��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageAnalyzerDoc)));
	return (CImageAnalyzerDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageAnalyzerView �T���B�z�`��


void CImageAnalyzerView::OnFileOpen()
{
	// TODO: �b���[�J�z���R�O�B�z�`���{���X
	CFileDialog dlg(TRUE, _T("Pictures"), _T("*.bmp"));
	BOOL        bOK;
	CSize       srcollSize;
	int         i;

	if (m_FileBMP.m_hFile != CFile::hFileNull) {
	    m_FileBMP.Close();
    }

	if (IDOK != dlg.DoModal()) {
		return;
	}

	m_MeasureCnt = 0;
	for (i = 0; i < 6; i++) {
		m_R[i] = 0;
		m_G[i] = 0;
		m_B[i] = 0;
	}

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

	if (m_hBMP) {
		m_bmpGDI.Detach();
		DeleteObject(m_hBMP);
		m_hBMP = NULL;
	}

	m_hBMP = (HBITMAP)::LoadImage(
		GetModuleHandle(NULL),
		m_strBMPFileName,
		IMAGE_BITMAP, 0, 0,
		LR_DEFAULTSIZE | LR_LOADFROMFILE);

	m_bmpGDI.Attach(m_hBMP);

	//
	// Set the size of scroll view
	//
	m_nBMPWidth = srcollSize.cx = m_bmp.GetWidth();
	m_nBMPHeight = srcollSize.cy = m_bmp.GetHeight();
            
	SetScrollSizes(MM_TEXT, srcollSize);

	Invalidate();
}


int CImageAnalyzerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �b���[�J�S�O�إߪ��{���X

	return 0;
}


void CImageAnalyzerView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �b���[�J�z���T���B�z�`���{���X
					   // ���n�I�s�Ϥ��T���� CScrollView::OnPaint()

	CDC            mDC;
	CBitmap       *p_bmp;
	CString        str;
	CPoint         point;

	point = GetScrollPosition();

	mDC.CreateCompatibleDC(&dc);
	p_bmp = mDC.SelectObject(&m_bmpGDI);
	dc.BitBlt(0, 0, m_nViewWidth, m_nViewHeight, &mDC, point.x, point.y, SRCCOPY);
	mDC.SelectObject(p_bmp);

	//str.Format(L"%d, %d : %d, %d, %d", m_nMeasureX, m_nMeasureY, m_nAVRGr, m_nAVRGg, m_nAVRGb);
	//dc.TextOutW(10, 10, str);
}


void CImageAnalyzerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: �b���[�J�z���T���B�z�`���{���X
	m_nViewWidth = cx;
	m_nViewHeight = cy;
}


void CImageAnalyzerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	CPoint ScrPoint;
	ScrPoint = GetScrollPosition();
	m_nMeasureX = ScrPoint.x + point.x;
	m_nMeasureY = ScrPoint.y + point.y;

	CScrollView::OnMouseMove(nFlags, point);
}


void CImageAnalyzerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	m_MeasurePoint1 = point;

	CScrollView::OnLButtonDown(nFlags, point);
}


void CImageAnalyzerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	CPoint ScrollPoint;
	int    nWidth, nHeight;

	if (m_MeasureCnt == 6) {
		return;
	}

	m_MeasurePoint2 = point;

	nWidth  = m_MeasurePoint2.x - m_MeasurePoint1.x;
	nHeight = m_MeasurePoint2.y - m_MeasurePoint1.y;

	ScrollPoint = GetScrollPosition();
	m_nMeasureX = ScrollPoint.x + m_MeasurePoint1.x;
	m_nMeasureY = ScrollPoint.y + m_MeasurePoint1.y;

	m_nAVRGr = m_bmp.GetAVG_R(m_nMeasureX, m_nMeasureY, nWidth, nHeight);
	m_nAVRGg = m_bmp.GetAVG_G(m_nMeasureX, m_nMeasureY, nWidth, nHeight);
	m_nAVRGb = m_bmp.GetAVG_B(m_nMeasureX, m_nMeasureY, nWidth, nHeight);

	if (m_MeasureCnt < 6) {
		m_R[m_MeasureCnt] = m_nAVRGr / 1000;
		m_G[m_MeasureCnt] = m_nAVRGg / 1000;
		m_B[m_MeasureCnt] = m_nAVRGb / 1000;
		m_MeasureCnt++;
	}

	if(m_MeasureCnt == 6)
	{
		int i;
		FILE *fp;

		fp = fopen("measure_result.txt", "r+t");
		if(!fp)	fp = fopen("measure_result.txt", "wt");
		fseek(fp, 0, SEEK_END);

		fprintf(fp, "\n");
		for (i = 0; i < m_strBMPFileName.GetLength(); i++) {
			fprintf(fp, "%c", m_strBMPFileName[i]);
		}
		fprintf(fp, "\nR");
		for (i = 0; i < 6; i++) {
			fprintf(fp, "\t%d", m_R[i]);
		}
		fprintf(fp, "\nG");
		for (i = 0; i < 6; i++) {
			fprintf(fp, "\t%d", m_G[i]);
		}
		fprintf(fp, "\nB");
		for (i = 0; i < 6; i++) {
			fprintf(fp, "\t%d", m_B[i]);
		}
		fclose(fp);
		fp = NULL;
	}

	Invalidate();

	CScrollView::OnLButtonUp(nFlags, point);
}
