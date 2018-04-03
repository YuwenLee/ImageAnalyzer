
// ImageAnalyzerView.cpp : CImageAnalyzerView Ãþ§Oªº?E@
//

#include "stdafx.h"
// SHARED_HANDLERS ¥i¥H©w¸q¦b?E@¹wÄý¡BÁY¹Ï©M·j´M¿z?Eø¥ó³B²z±`¦¡ªº
// ATL ±M®×¤¤¡A¨Ã¤¹³\»P¸Ó±M®×¦@¥Î¤å¥óµ{¦¡½X¡C
#ifndef SHARED_HANDLERS
#include "ImageAnalyzer.h"
#endif

#include "ImageAnalyzerDoc.h"
#include "ImageAnalyzerView.h"
#include "Img_RAW.h"
#include "RawFormatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageAnalyzerView

IMPLEMENT_DYNCREATE(CImageAnalyzerView, CScrollView)

BEGIN_MESSAGE_MAP(CImageAnalyzerView, CScrollView)
	ON_COMMAND(ID_FILE_OPEN, &CImageAnalyzerView::OnFileOpen)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_ACTION_TEACHERDATA, &CImageAnalyzerView::OnActionTeacherdata)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TEACHERDATA, &CImageAnalyzerView::OnUpdateActionTeacherdata)
	ON_UPDATE_COMMAND_UI(ID_ACTION_MEASURE, &CImageAnalyzerView::OnUpdateActionMeasure)
	ON_COMMAND(ID_ACTION_MEASURE, &CImageAnalyzerView::OnActionMeasure)
END_MESSAGE_MAP()

// CImageAnalyzerView «Øºc/¸Ñºc

CImageAnalyzerView::CImageAnalyzerView()
	: m_strInputFileName(_T(""))
	, m_strBMPFileName(_T(""))
	, m_hBMP(NULL)
	, m_bGotButtonDown(FALSE)
	, m_nBMPWidth(0)
	, m_nBMPHeight(0)
	, m_nViewWidth(0)
	, m_nViewHeight(0)
	, m_nMeasureX(0)
	, m_nMeasureY(0)
	, m_nAVRGr(0)
	, m_nAVRGb(0)
	, m_nAVRGg(0)
	, m_MeasureCnt(0)
{
	// TODO: ¦b¦¹¥[¤J«Øºcµ{¦¡½X
	int i;
	for (i = 0; i < 6; i++) {
		m_B[i] = 0;
		m_G[i] = 0;
		m_R[i] = 0;
		m_L[i] = 0.000001;
	}

	m_nAction = 1; // Measure RGB
}

CImageAnalyzerView::~CImageAnalyzerView()
{
}

BOOL CImageAnalyzerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ¦b¦¹¸g¥Ñ­×?ECREATESTRUCT cs 
	// ¹F?E×§Eøµ¡Ãþ§O©Î¼Ë¦¡ªº¥Øªº

	return CScrollView::PreCreateWindow(cs);
}

// CImageAnalyzerView ´yÃ¸

void CImageAnalyzerView::OnDraw(CDC* pDC)
{
	CImageAnalyzerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ¦b¦¹¥[¤J?EÍ¸EÆªº´yÃ¸µ{¦¡½X
	CDC            dcMem;
	CBitmap       *p_bmp;
	CString        str;
	CPoint         point;
	CPen          *pPen;
	CPen           pen(0, 1, COLORREF(0x00FF0000));
	int            i;

	point = GetScrollPosition();

	// Load BMP file to the compatible DC and copy the content to the paint DC.
	dcMem.CreateCompatibleDC(pDC);
	p_bmp = dcMem.SelectObject(&m_bmpGDI);
	pDC->BitBlt(point.x, point.y, m_nViewWidth, m_nViewHeight, &dcMem, point.x, point.y, SRCCOPY);
	dcMem.SelectObject(p_bmp);

	pPen = pDC->SelectObject(&pen);
	for (i = 0; i < m_MeasureCnt; i++) {
		CPoint p1, p2;
		p1.x = m_RectMeasureRGB[i].left;
		p1.y = m_RectMeasureRGB[i].top;
		p2.x = m_RectMeasureRGB[i].right;
		p2.y = m_RectMeasureRGB[i].bottom;
		DrawRect(pDC, p1, p2);
	}
	pDC->SelectObject(pPen);
	/*
	for (i = 0; i < 6; i++) {
		str.Format(_T("[%d] (%d, %d, %d)"), i, m_R[i], m_G[i], m_B[i]);
		pDC->TextOut(point.x + 5, point.y + 5 + i * 18, str);
	}
	*/
	
	//str.Format(L"%d, %d : %d, %d, %d", m_nMeasureX, m_nMeasureY, m_nAVRGr, m_nAVRGg, m_nAVRGb);
	//dc.TextOutW(10, 10, str);
}

void CImageAnalyzerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: ­pºâ¦¹ÀËµøªºÁ`¤j¤p
	sizeTotal.cx = 100;
	sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	
}


// CImageAnalyzerView ¶EÂ_

#ifdef _DEBUG
void CImageAnalyzerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CImageAnalyzerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CImageAnalyzerDoc* CImageAnalyzerView::GetDocument() const // ¤º´O«D°»¿ùª©¥»
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageAnalyzerDoc)));
	return (CImageAnalyzerDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageAnalyzerView °T®§³B²z±`¦¡


void CImageAnalyzerView::OnFileOpen()
{
	// TODO: ¦b¦¹¥[¤J±zªº©R¥O³B²z±`¦¡µ{¦¡½X
	TCHAR       szFileFilters_JPG[] = _T("Pictures (*.jpg)|*.jpg;*.jpeg|Pictures (*.bmp)|*.bmp||");
	TCHAR       szFileFilters_RAW[] = _T("RAW Data|*.raw||");
	BOOL        bOK;
	CSize       srcollSize;
	int         i, j;
	int         pic_type = 0;
	TCHAR       szTempPath[MAX_PATH + 1];

	if (m_nAction == 0) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileFilters_RAW);
		if (IDOK != dlg.DoModal()) {
			return;
		}
		m_strInputFileName = dlg.GetPathName();
	}
	else if(m_nAction == 1) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileFilters_JPG);
		if (IDOK != dlg.DoModal()) {
			return;
		}
		m_strInputFileName = dlg.GetPathName();
	}

	if (m_FileBMP.m_hFile != CFile::hFileNull) {
		m_FileBMP.Close();
	}


	//
	// Clean Up
	//
	if (m_strResultFile.GetLength()) {
		m_strResultFile.Empty();
	}

	m_MeasureCnt = 0;
	for (i = 0; i < 6; i++) {
		m_R[i] = 0;
		m_G[i] = 0;
		m_B[i] = 0;
	}

	GetTempPath(MAX_PATH + 1, szTempPath);
	m_strBMPFileName = szTempPath;
	m_strBMPFileName += "ClientDC.bmp";

	//
	// Check the input file name and generate output file name
	//
	j = m_strInputFileName.GetLength();
	if (((m_strInputFileName.GetAt(j - 3) & 0xDF) == 'J') &&
		((m_strInputFileName.GetAt(j - 2) & 0xDF) == 'P') &&
		((m_strInputFileName.GetAt(j - 1) & 0xDF) == 'G'))
	{
		GenerateBMP(m_strInputFileName, m_strBMPFileName);
		pic_type = 1;
	}
	else if (((m_strInputFileName.GetAt(j - 3) & 0xDF) == 'B') &&
             ((m_strInputFileName.GetAt(j - 2) & 0xDF) == 'M') &&
             ((m_strInputFileName.GetAt(j - 1) & 0xDF) == 'P'))
	{
		m_strBMPFileName = m_strInputFileName;
		pic_type = 2;
	}
	else if (((m_strInputFileName.GetAt(j - 3) & 0xDF) == 'R') &&
	         ((m_strInputFileName.GetAt(j - 2) & 0xDF) == 'A') &&
	         ((m_strInputFileName.GetAt(j - 1) & 0xDF) == 'W'))
	{
		int     i, j;

		m_strBMPFileName = m_strInputFileName;
		for (i = j = m_strInputFileName.GetLength(); i; i--) {
			if (m_strBMPFileName[i] == '.') {
				break;
			}
		}

		m_strBMPFileName.GetBuffer()[++i] = 'b';
		m_strBMPFileName.GetBuffer()[++i] = 'm';
		m_strBMPFileName.GetBuffer()[++i] = 'p';

		pic_type = 3;

		GenerateBMP_from_RAW(m_strInputFileName, m_strBMPFileName);
	}

	//
	// Extract EXIF
	//
	switch (pic_type)
	{
		default:
		case 1:
		case 2:
		case 3:
			bOK = m_FileBMP.Open(m_strBMPFileName, CFile::modeRead | CFile::typeBinary);
			if (!bOK) {
				return;
			}
			//
			// Prepare for the new result file name
			//
			{
				j = m_strInputFileName.GetLength();
				m_strResultFile.GetBufferSetLength(j);
				for (i = 0; i < (j - 3); i++) {
					m_strResultFile.SetAt(i, m_strInputFileName[i]);
				}

				m_strResultFile.SetAt(i++, 't');
				m_strResultFile.SetAt(i++, 'x');
				m_strResultFile.SetAt(i, 't');
			}

			//
			// Load the Image File
			//
			{
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
			}
			break;
	}

	Invalidate();
}


int CImageAnalyzerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ¦b¦¹¥[¤J¯S§O«Ø¥ßªºµ{¦¡½X

	return 0;
}

void CImageAnalyzerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: ¦b¦¹¥[¤J±zªº°T®§³B²z±`¦¡µ{¦¡½X
	m_nViewWidth = cx;
	m_nViewHeight = cy;
}


void CImageAnalyzerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ¦b¦¹¥[¤J±zªº°T®§³B²z±`¦¡µ{¦¡½X©M (©Î) ©I¥s¹w³]­È
	CPoint ScrPoint;
	ScrPoint = GetScrollPosition();
	m_nMeasureX = ScrPoint.x + point.x;
	m_nMeasureY = ScrPoint.y + point.y;

	if (m_bGotButtonDown) {
		CClientDC dc(this);
		CPen      pen(2, 1, COLORREF(0x00FFFF00));
		CPen     *pPen;
		
		RecoverRect(&dc, m_MeasurePoint1, m_MeasurePoint2);
		pPen = dc.SelectObject(&pen);
		DrawRect(&dc, m_MeasurePoint1, point);
		dc.SelectObject(pPen);

		m_MeasurePoint2 = point;
	}


	CScrollView::OnMouseMove(nFlags, point);
}


void CImageAnalyzerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ¦b¦¹¥[¤J±zªº°T®§³B²z±`¦¡µ{¦¡½X©M (©Î) ©I¥s¹w³]­È
	m_bGotButtonDown = TRUE;
	m_MeasurePoint1 = point;

	CScrollView::OnLButtonDown(nFlags, point);
}


void CImageAnalyzerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ¦b¦¹¥[¤J±zªº°T®§³B²z±`¦¡µ{¦¡½X©M (©Î) ©I¥s¹w³]­È
	CPoint ScrollPoint;
	int    nWidth, nHeight;

	if (!m_bGotButtonDown) {
		return;
	}
	m_bGotButtonDown = FALSE;
	Invalidate();

	if (m_MeasurePoint1 == point) {
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
		m_RectMeasureRGB[m_MeasureCnt].left = m_nMeasureX;
		m_RectMeasureRGB[m_MeasureCnt].top = m_nMeasureY;
		m_RectMeasureRGB[m_MeasureCnt].right = m_nMeasureX + nWidth;
		m_RectMeasureRGB[m_MeasureCnt].bottom = m_nMeasureY + nHeight;

		m_R[m_MeasureCnt] = m_nAVRGr / 1000;
		m_G[m_MeasureCnt] = m_nAVRGg / 1000;
		m_B[m_MeasureCnt] = m_nAVRGb / 1000;

		m_L[m_MeasureCnt] = pow( (m_R[m_MeasureCnt]*m_R[m_MeasureCnt]*0.299) + 
			                     (m_G[m_MeasureCnt]*m_G[m_MeasureCnt]*0.587) + 
			                     (m_B[m_MeasureCnt]*m_B[m_MeasureCnt]*0.114), 
			                     0.5 );
		m_MeasureCnt++;
	}

	/*
	 * Output 1 (measure_result.txt)
	 */
	if (0) {
		if (m_MeasureCnt == 6)
		{
			int i;
			FILE *fp;

			fp = fopen("measure_result.txt", "r+t");
			if (!fp)	fp = fopen("measure_result.txt", "wt");
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
	}
	/*
	 * Output 2 (for MySQL)
	 */
	if (0) {
		if (m_MeasureCnt == 6) {
			int i;
			FILE *fp;

			fp = fopen("measure_result_sql.txt", "r+t");
			if (!fp)	fp = fopen("measure_result_sql.txt", "wt");
			fseek(fp, 0, SEEK_END);

			fprintf(fp, "\n");
			for (i = 0; i < m_strInputFileName.GetLength(); i++) {
				if (m_strInputFileName[i] == '\n') break;
				fprintf(fp, "%c", m_strInputFileName[i]);
			}
			fprintf(fp, "\t");
			//fprintf(fp, "\nR");
			for (i = 0; i < 6; i++) {
				fprintf(fp, "\t%d", m_R[i]);
				fprintf(fp, "\t%d", m_G[i]);
				fprintf(fp, "\t%d", m_B[i]);
			}
			fclose(fp);
			fp = NULL;
		}
	}
	//Invalidate();

	CScrollView::OnLButtonUp(nFlags, point);
}

void CImageAnalyzerView::SaveResult(CString strFilename)
{
	CStdioFile     file;
	int            nOK;

	if (!file.Open(strFilename, CStdioFile::modeReadWrite | CStdioFile::typeText)) {
		// The file does not exist. Create it.
		nOK = file.Open(strFilename, CStdioFile::modeCreate | CStdioFile::modeReadWrite | CStdioFile::typeText);
		if (!nOK) {
			// Failed to create the file
			return;
		}
	}

	file.Seek(0, CStdioFile::end);

	// Parse EXIF
	do {
		CString    str;
		char       szbuf[1024];
		int        i, j, k;

		// exiftool -ExposureTime -ISO -FNUMBER -CREATEDATE img001.jpg > exif.txt
		str.Format(_T(".\\exiftool -ExposureTime -ISO -FNUMBER -CREATEDATE "));
		str += '\"';
		str += m_strInputFileName;
		str += '\"';
		str += _T(" > exif.txt");

		if ((j = str.GetLength()) > 1024) {
			break;
		}
		for (i = 0; i < j; i++) {
			szbuf[i] = (char)(str[i]);
		}
		szbuf[i] = 0;
		system(szbuf);

		str = getValue(_T("exif.txt"), _T("Exposure Time"));
		j = str.GetLength();
		for (i = 0; i < j; i++) {
			if (str[i] == '/') {
				break;
			}
		}
		i++;
		k = 0;
		while (i < j) {
			szbuf[k++] = (char)(str[i++]);
			if ((str[i] == ' ') || (str[i] == '\n') || (str[i] == '\0')) {
				break;
			}
		}
		szbuf[k] = '\0';
		m_nExposure = atoi(szbuf);

		str = getValue(_T("exif.txt"), _T("ISO"));
		j = str.GetLength();
		for (i = 0; i < j; i++) {
			szbuf[i] = (char)(str[i]);
			if ((str[i] == ' ') || (str[i] == '\n') || (str[i] == '\0')) {
				break;
			}
		}
		szbuf[i] = '\0';
		m_nISO = atoi(szbuf);

		str = getValue(_T("exif.txt"), _T("F Number"));
		j = str.GetLength();
		for (i = 0; i < j; i++) {
			szbuf[i] = (char)(str[i]);
			if (str[i] == '.') {
				break;
			}
		}
		szbuf[i++] = '\0';
		m_nFnum_n = atoi(szbuf);
		for (k = 0; i < j; i++, k++) {
			szbuf[k] = (char)(str[i]);
			if ((str[i] == ' ') || (str[i] == '\n') || (str[i] == '\0')) {
				break;
			}
		}
		szbuf[k] = '\0';
		m_nFnum_f = atoi(szbuf);

		str = getValue(_T("exif.txt"), _T("Create Date"));
		j = str.GetLength();
		for (i = 0; i < j; i++) {
			szbuf[i] = (char)(str[i]);
			if ((str[i] == '\n') || (str[i] == '\0')) {
				break;
			}
		}
		szbuf[i] = '\0';
		sscanf(szbuf, "%d:%d:%d %d:%d:%d", &m_nYear, &m_nMonth, &m_nDay, &m_nHour, &m_nMin, &m_nSec);
		break;
	} while (0);

	//
	// Output (ImageName.txt)
	//
	{
		CString str;
		str.Format(_T("[Filename]\n%s\n[Date]\n%d-%d-%d-%d-%d-%d\n"), m_strInputFileName, m_nYear, m_nMonth, m_nDay, m_nHour, m_nMin, m_nSec);
		file.WriteString(str);
		str.Format(_T("[Brightness]\nExpo\tISO\tF#\n"));
		file.WriteString(str);
		str.Format(_T("1/%d \t%d \t%d.%d\n"), m_nExposure, m_nISO, m_nFnum_n, m_nFnum_f);
		file.WriteString(str);
	}

	//
	// Output (measure_result.txt)
	//
	{
		int i;
		FILE *fp;

		fp = fopen("measure_result_1.0.0.2.txt", "r+t");
		if (!fp)	fp = fopen("measure_result_1.0.0.2.txt", "wt");
		fseek(fp, 0, SEEK_END);

		fprintf(fp, "\n");
		for (i = 0; i < m_strBMPFileName.GetLength(); i++) {
			fprintf(fp, "%c", m_strBMPFileName[i]);
		}

		for (i = 0; i < 6; i++) {
			fprintf(fp, "\t%d", m_R[i]);
			fprintf(fp, "\t%d", m_G[i]);
			fprintf(fp, "\t%d", m_B[i]);
		}

		for (i = 0; i < 6; i++) {
			fprintf(fp, "\t%.f", m_L[i]);
		}

		fprintf(fp, "\t%d\t%d\t%d\t%d", m_nExposure, m_nISO, m_nFnum_n, m_nFnum_f);

		fclose(fp);
		fp = NULL;
	}

	{
		CString str1;
		CString str2;
		int     i;

		file.WriteString(L"[GrayPatches]\n");
		str1.Format(L" \t1 \t2 \t3 \t4 \t5 \t6\n");
		file.WriteString(str1);

		str1.Empty();
		str2.Empty();		
		file.WriteString(L"x\t");
		for (i = 0; i < 6; i++) {
			str2.Format(L"%d \t", m_RectMeasureRGB[i].left);
			str1 += str2;
		}
		str1 += "\n";
		file.WriteString(str1);
		
		str1.Empty();
		file.WriteString(L"y\t");
		for (i = 0; i < 6; i++) {
			str2.Format(L"%d \t", m_RectMeasureRGB[i].top);
			str1 += str2;
		}
		str1 += "\n";
		file.WriteString(str1);

		str1.Empty();
		file.WriteString(L"w\t");
		for (i = 0; i < 6; i++) {
			str2.Format(L"%d \t", m_RectMeasureRGB[i].right - m_RectMeasureRGB[i].left);
			str1 += str2;
		}
		str1 += "\n";
		file.WriteString(str1);

		str1.Empty();
		file.WriteString(L"h\t");
		for (i = 0; i < 6; i++) {
			str2.Format(L"%d \t", m_RectMeasureRGB[i].bottom - m_RectMeasureRGB[i].top);
			str1 += str2;
		}
		str1 += "\n";
		file.WriteString(str1);

		file.WriteString(L"[GrayPatch_Measurement]\n");
		str1.Format(L"\t19 \t20 \t21 \t22 \t23 \t24\n");
		file.WriteString(str1);

		str1.Format(L"R \t%d \t%d \t%d \t%d \t%d \t%d\n", m_R[0], m_R[1], m_R[2], m_R[3], m_R[4], m_R[5]);
		file.WriteString(str1);
		str1.Format(L"G \t%d \t%d \t%d \t%d \t%d \t%d\n", m_G[0], m_G[1], m_G[2], m_G[3], m_G[4], m_G[5]);
		file.WriteString(str1);
		str1.Format(L"B \t%d \t%d \t%d \t%d \t%d \t%d\n", m_B[0], m_B[1], m_B[2], m_B[3], m_B[4], m_B[5]);
		file.WriteString(str1);
		str1.Format(L"L \t%.f \t%.f \t%.f \t%.f \t%.f \t%.f\n", m_L[0], m_L[1], m_L[2], m_L[3], m_L[4], m_L[5]);
		file.WriteString(str1);
	}

	file.Close();

	/*
	 * Peform SQL Commands
	 */
	if(0) {
		CStdioFile sql_script;
		CString    strInputFileName;
		CString    sql_script_name(L"D:\\Sql_Script.sql");
		CString    strBegin("\
USE YWLEE;\n\
INSERT INTO measure_result(\n\
`Picture_Name`, \n\
`R19`, `G19`, `B19`, `R20`, `G20`, `B20`, `R21`, \n\
`G21`, `B21`, `R22`, `G22`, `B22`, `R23`, `G23`, \n\
`B23`, `R24`, `G24`, `B24`, \n\
`L19`, `L20`, `L21`, `L22`, `L23`, `L24`, \n\
`Target_Image_ID`) \n\
VALUES(\n");
		CString strValues;
		CString strEnd("0 ) ; ");
		CString strCmd;
		int     i;

		strValues.Empty();
		strInputFileName = m_strInputFileName;
		for (i = 0; i < strInputFileName.GetLength(); i++) {
			if (strInputFileName.GetBuffer()[i] == '\\') {
				strInputFileName.GetBuffer()[i] = '\/';
			}
		}
		strValues.Format(L"'%s', ", strInputFileName);

		for (i = 0; i < 6; i++) {
			CString str;
			str.Format(L"%d, %d, %d, ", m_R[i], m_G[i], m_B[i]);
			strValues += str;
		}

		for (i = 0; i < 6; i++) {
			CString str;
			int     nLight;
			nLight = pow((m_R[i] * m_R[i] * 0.299) + (m_G[i] * m_G[i] * 0.587) + (m_B[i] * m_B[i] * 0.114), 0.5);
			str.Format(L"%d, ", nLight);
			strValues += str;
		}

		//MessageBox(L"Open", 0);
		if (!sql_script.Open(sql_script_name, CStdioFile::modeReadWrite | CStdioFile::typeText)) {
			//MessageBox(L"File does not exist", 0);
			// The file does not exist. Create it.
			nOK = sql_script.Open(sql_script_name, CStdioFile::modeCreate | CStdioFile::modeReadWrite | CStdioFile::typeText);
			if (!nOK) {
				// Failed to create the file
				MessageBox(L"Fail", 0);
				return;
			}
		}

		sql_script.SetLength(0);
		sql_script.Seek(0, CStdioFile::begin);
		sql_script.WriteString(strBegin);
		sql_script.WriteString(strValues);
		sql_script.WriteString(strEnd);

		sql_script.Close();

		
		strCmd = L"\"c:\\Program Files\\MySQL\\MySQL Server 5.7\\bin\\mysql.exe\" -h127.0.0.1 -uroot -p70256 < ";
		strCmd += sql_script_name;

		_wsystem(strCmd);
	}
	return;
}

int CImageAnalyzerView::GenerateBMP(CString strFileName, CString strBMPFileName)
{
	//CString strBMPFileName(_T(""));
	CString strCmd;
	char    szCmd[1024];
	int     i, j;

	/*
	strBMPFileName = strFileName;
	for (i = j = m_strInputFileName.GetLength(); i; i--) {
		if (strBMPFileName[i] == '.') {
			break;
		}
	}
	if (i == 0) {
		return strBMPFileName;
	}
	
	strBMPFileName.GetBuffer()[++i] = 'b';
	strBMPFileName.GetBuffer()[++i] = 'm';
	strBMPFileName.GetBuffer()[++i] = 'p';
	*/

	// ffmpeg -i IMG20170712151728.jpg -frames 1 -pix_fmt bgr24 -y IMG20170712151728.bmp
	strCmd.Format(_T(".\\ffmpeg -i "));
	strCmd += '\"'; 
	strCmd += strFileName;
	strCmd += '\"';
	strCmd += _T(" -vframes 1 -pix_fmt bgr24 -y ");
	strCmd += '\"';
	strCmd += strBMPFileName;
	strCmd += '\"';

	if (strCmd.GetLength() > 1023) {
		return -1;
	}

	for (i = 0; i < strCmd.GetLength(); i++) {
		szCmd[i] = (char)(strCmd[i]);
	}
	szCmd[i] = 0;
	system(szCmd);

	return 0;
}

int CImageAnalyzerView::GenerateBMP_from_RAW(CString strFileName, CString strBMPFileName)
{
	CFile         cfile;
	Img_RAW       raw;
	BMP           bmp;
	CRawFormatDlg dlg;
	int           format = 0, width = 0, height = 0;
	CString       strOutputFilename;
	int           i;

	cfile.Open(m_strInputFileName, CFile::modeRead | CFile::typeBinary);
	raw.SetBufferSize(cfile.GetLength());
	cfile.Read(raw.GetBuffer(), cfile.GetLength());
	if (IDOK == dlg.DoModal()) {
		format = dlg.GetFormat();
		dlg.GetWidthHeight(&width, &height);
	}
	raw.SetFormat(format, width, height);

	bmp.SetBufferSize(width, height);
	for (i = 0; i < height; i++) {
		bmp.SetLine(raw.GetRGB()+i*width*3, i);
	}

	{
		FILE *fp;
		fp = fopen("test-kill.bmp", "wb");
		fwrite(bmp.GetBuffer(), bmp.GetBufferSize(), 1, fp);
		fclose(fp);
	}
	//raw.WriteToBMP(m_strBMPFileName);

	return 0;
}

CString CImageAnalyzerView::getValue(CString strFilename, CString strTag)
{
	CString    strVal;
	CString    buf;
	CStdioFile file;
	int        i, j, k;
	
	strVal.Empty();
	if (!file.Open((LPCTSTR)strFilename, CStdioFile::modeRead | CStdioFile::typeText)) {
		return strVal;
	}

	buf.GetBufferSetLength(128);
	do {
		if (!file.ReadString(buf.GetBuffer(), 128)) {
			break;
		}
		i = buf.Find(strTag);
		if (i == -1) {
			continue;
		}
		j = buf.Find(_T(":"), i);
		if (j == -1) {
			break;
		}
		j += 1;
		strVal.GetBufferSetLength(64);
		// Bypass spaces
		k = buf.GetLength();
		while(j < k) {
			if (buf[j] == ' ' || buf[j] == '\n' || buf[j] == '\r' || buf[j] == '\t') {
				j++;
				continue;
			}
			break;
		}
		for (i = 0; i < 64; i++, j++) {
			if (buf[j] == '\n' || buf[j] == '\r') {
				break;
			}
			strVal.GetBuffer()[i] = buf[j];
		}
		strVal.GetBuffer()[i] = '\0';
	} while (1);
     
	file.Close();
    return strVal;
}


void CImageAnalyzerView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ¦b¦¹¥[¤J±zªº°T®§³B²z±`¦¡µ{¦¡½X©M (©Î) ©I¥s¹w³]­È
	if (nChar == 'S') {
		//
		// Save last time result
		//
		if (m_strResultFile.GetLength()) {
			SaveResult(m_strResultFile);
			m_strResultFile.Empty();
		}
		MessageBox(L"The Result has been saved.", L"OK", 0);

	}

	if (nChar == 'R') {
		// Re-measure
		int i;
		m_MeasureCnt = 0;
		for (i = 0; i < 6; i++) {
			m_R[i] = 0;
			m_G[i] = 0;
			m_B[i] = 0;
		}
		Invalidate();
	}

	// Move Picture
	if (nChar == 'M') {
		CString cmd;
		CString dir;
		int     i, l;

		for (i = m_strInputFileName.GetLength(); i >= 0; i--) {
			if (m_strInputFileName[i] == '\\') break;
		}
		l = i + 1;
		dir.GetBufferSetLength(l);
		for (i = 0; i < l; i++) {
			dir.GetBuffer()[i] = m_strInputFileName[i];
		}
		cmd.Format(L"move %s %s\M\\", m_strInputFileName, dir);
		_wsystem(cmd);
	}

	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CImageAnalyzerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ¦b¦¹¥[¤J±zªº°T®§³B²z±`¦¡µ{¦¡½X©M (©Î) ©I¥s¹w³]­È
	//Invalidate();
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CImageAnalyzerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ¦b¦¹¥[¤J±zªº°T®§³B²z±`¦¡µ{¦¡½X©M (©Î) ©I¥s¹w³]­È
	//Invalidate();
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}


int CImageAnalyzerView::DrawRect(CDC *pDC, CPoint p1, CPoint p2)
{
	int top, bottom, right, left;

	if (p1.y < p2.y) {
		top = p1.y;
		bottom = p2.y;
	}
	else {
		top = p2.y;
		bottom = p1.y;
	}

	if (p1.x < p2.x) {
		left = p1.x;
		right = p2.x;
	}
	else {
		left = p2.x;
		right = p1.x;
	}

	pDC->MoveTo(left, top);
	pDC->LineTo(right, top);
	pDC->LineTo(right, bottom);
	pDC->LineTo(left,  bottom);
	pDC->LineTo(left,  top);

	return 0;
}

int CImageAnalyzerView::RecoverRect(CDC *pDC, CPoint p1, CPoint p2)
{
	CRect rect;
	//int   top, bottom, right, left;

	if (p1.y < p2.y) {
		rect.top = p1.y + 1;
		rect.bottom = p2.y;
	}
	else {
		rect.top = p2.y + 1;
		rect.bottom = p1.y;
	}

	if (p1.x < p2.x) {
		rect.left = p1.x + 1;
		rect.right = p2.x;
	}
	else {
		rect.left = p2.x + 1;
		rect.right = p1.x;
	}

	InvalidateRect(rect);

	return 0;
}

void CImageAnalyzerView::OnActionTeacherdata()
{
	// TODO: ¦b¦¹¥[¤J±zªº©R¥O³B²z±`¦¡µ{¦¡½X
	m_nAction = 0;
	return;
}

void CImageAnalyzerView::OnUpdateActionTeacherdata(CCmdUI *pCmdUI)
{
	// TODO: ¦b¦¹¥[¤J±zªº©R¥O§ó·s UI ³B²z±`¦¡µ{¦¡½X
	if (m_nAction == 0) {
		pCmdUI->SetCheck(1);
	}
	else {
		pCmdUI->SetCheck(0);
	}
}

void CImageAnalyzerView::OnActionMeasure()
{
	// TODO: ¦b¦¹¥[¤J±zªº©R¥O³B²z±`¦¡µ{¦¡½X
	m_nAction = 1;

}

void CImageAnalyzerView::OnUpdateActionMeasure(CCmdUI *pCmdUI)
{
	// TODO: ¦b¦¹¥[¤J±zªº©R¥O§ó·s UI ³B²z±`¦¡µ{¦¡½X
	if (m_nAction == 1) {
		pCmdUI->SetCheck(1);
	}
	else {
		pCmdUI->SetCheck(0);
	}
}
