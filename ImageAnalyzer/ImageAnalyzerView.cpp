
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
#include "Img_RAW.h"
#include "RawFormatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum ImageAnalyzer_Action {
	Action_MeasuringCalibrationData = 0,
	Action_MeasuringTeacherData,
	Action_MeasuringRGB
};

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
	ON_COMMAND(ID_ZOOM_IN, &CImageAnalyzerView::OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CImageAnalyzerView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ACTION_CALIBRATION, &CImageAnalyzerView::OnUpdateActionCalibration)
	ON_COMMAND(ID_ACTION_CALIBRATION, &CImageAnalyzerView::OnActionCalibration)
	ON_COMMAND(ID_FILE_SAVE_AS, &CImageAnalyzerView::OnFileSaveAs)
END_MESSAGE_MAP()

// CImageAnalyzerView 建構/解構

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
	// TODO: 在此加入建構程式碼
	CString    str;
	char       caLine[512];
	int        i, j;

	for (i = 0; i < 6; i++) {
		m_B[i] = 0;
		m_G[i] = 0;
		m_R[i] = 0;
		m_L[i] = (float)0.0001;
	}

	m_nAction = Action_MeasuringRGB; // Measure RGB
	m_nZoomPercent = 100; // Picture's original size

	//
	// RAW Data Format
	//
	str = getValue(L"cfg.txt", L"RAW Dlg Format");
	m_nRAWDlg_format = _ttoi(str);
	str = getValue(L"cfg.txt", L"RAW Dlg Width");
	m_nRAWDlg_width = _ttoi(str);
	str = getValue(L"cfg.txt", L"RAW Dlg Height");
	m_nRAWDlg_height = _ttoi(str);

	//
	// Calibration Data
	//
	m_nCalibrationCnt = 0;
	m_nVertex = 0;
	for (i = 0; i < 20; i++) {
		m_strILL_Name[i].Empty();
		m_nILL_CTT[i] = 0;
		m_fduv[i]     = 0.0;
		for (j = 0; j < 4; j++) {
			m_MCC_Vertices[i][j].x = 0;
			m_MCC_Vertices[i][j].y = 0;
		}
		m_ptCal[i][0].x = 0;
		m_ptCal[i][0].y = 0;
		m_ptCal[i][1].x = 0;
		m_ptCal[i][1].y = 0;
		m_nCalR[i] = 0;
		m_nCalG[i] = 0;
		m_nCalB[i] = 0;
	}
}

CImageAnalyzerView::~CImageAnalyzerView()
{
	setValue(L"cfg.txt", L"RAW Dlg Format", m_nRAWDlg_format);
	setValue(L"cfg.txt", L"RAW Dlg Width",  m_nRAWDlg_width);
	setValue(L"cfg.txt", L"RAW Dlg Height", m_nRAWDlg_height);
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
	//pDC->BitBlt(point.x, point.y, m_nViewWidth, m_nViewHeight, &dcMem, point.x, point.y, SRCCOPY);
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(point.x, point.y, 
		            m_nViewWidth, m_nViewHeight,
		            &dcMem, 
					point.x*100/m_nZoomPercent, point.y*100/m_nZoomPercent,
					m_nViewWidth*100/m_nZoomPercent, m_nViewHeight*100/m_nZoomPercent,
		            SRCCOPY);
	dcMem.SelectObject(p_bmp);

	//
	// Measuring RGB
	//
	if (m_nAction == Action_MeasuringRGB) {
		pPen = pDC->SelectObject(&pen);
		for (i = 0; i < m_MeasureCnt; i++) {
			CPoint p1, p2;
			p1.x = m_RectMeasureRGB[i].left*m_nZoomPercent / 100;
			p1.y = m_RectMeasureRGB[i].top*m_nZoomPercent / 100;
			p2.x = m_RectMeasureRGB[i].right*m_nZoomPercent / 100;
			p2.y = m_RectMeasureRGB[i].bottom*m_nZoomPercent / 100;
			DrawRect(pDC, p1, p2);
		}
		pDC->SelectObject(pPen);
	}

	//
	// Measuring Calibration Data
	//
	if (m_nAction == Action_MeasuringCalibrationData) {
		if (m_nVertex == 1) {
			pDC->SetPixel(m_MCC_Vertices[m_nCalibrationCnt][0].x*m_nZoomPercent / 100,
				m_MCC_Vertices[m_nCalibrationCnt][0].y*m_nZoomPercent / 100,
				RGB(0, 0, 255));
		}
		pPen = pDC->SelectObject(&pen);
		for (i = 1; i < m_nVertex; i++) {
			pDC->MoveTo(m_MCC_Vertices[m_nCalibrationCnt][i - 1].x*m_nZoomPercent / 100, m_MCC_Vertices[m_nCalibrationCnt][i - 1].y*m_nZoomPercent / 100);
			pDC->LineTo(m_MCC_Vertices[m_nCalibrationCnt][i].x*m_nZoomPercent / 100, m_MCC_Vertices[m_nCalibrationCnt][i].y*m_nZoomPercent / 100);
		}
		if (m_nVertex == 4) {
			pDC->MoveTo(m_MCC_Vertices[m_nCalibrationCnt][3].x*m_nZoomPercent / 100, m_MCC_Vertices[m_nCalibrationCnt][3].y*m_nZoomPercent / 100);
			pDC->LineTo(m_MCC_Vertices[m_nCalibrationCnt][0].x*m_nZoomPercent / 100, m_MCC_Vertices[m_nCalibrationCnt][0].y*m_nZoomPercent / 100);
		}
		if (m_nCalR[m_nCalibrationCnt] != 0) {
			CPoint p1, p2;
			p1.x = m_ptCal[m_nCalibrationCnt][0].x*m_nZoomPercent/100;
			p1.y = m_ptCal[m_nCalibrationCnt][0].y*m_nZoomPercent/100;
			p2.x = m_ptCal[m_nCalibrationCnt][1].x*m_nZoomPercent/100;
			p2.y = m_ptCal[m_nCalibrationCnt][1].y*m_nZoomPercent/100;
			DrawRect(pDC, p1, p2);
		}
		pDC->SelectObject(pPen);
	}

	/*
	for (i = 0; i < 6; i++) {
		str.Format(_T("[%d] (%d, %d, %d)"), i, m_R[i], m_G[i], m_B[i]);
		pDC->TextOut(point.x + 5, point.y + 5 + i * 18, str);
	}
	*/
	
	//str.Format(L"%d, %d : %d, %d, %d", m_nMeasureX, m_nMeasureY, m_nAVRGr, m_nAVRGg, m_nAVRGb);
	//dc.TextOutW(10, 10, str);
}

int CImageAnalyzerView::LoadBMPFile(CString strFileName)
{
	unsigned int uSize;
	CSize        size;
	BOOL         bOK;

	bOK = m_FileBMP.Open(strFileName, CFile::modeRead | CFile::typeBinary);
	if (!bOK) {
		return -1;
	}

	//
	// Read the BMP file
	//
	uSize = (unsigned int)m_FileBMP.GetLength();
	m_bmp.SetBufferSize(uSize);
	m_FileBMP.Read(m_bmp.GetFileHeader(), uSize);
	m_FileBMP.Close();

	m_nBMPWidth  = m_bmp.GetWidth();
	m_nBMPHeight = m_bmp.GetHeight();

	//
	// Attach the BMP file to GDI object
	//
	if (m_hBMP) {
		m_bmpGDI.Detach();
		DeleteObject(m_hBMP);
		m_hBMP = NULL;
	}

	m_hBMP = (HBITMAP)::LoadImage(
		GetModuleHandle(NULL),
		strFileName,
		IMAGE_BITMAP, 0, 0,
		LR_DEFAULTSIZE | LR_LOADFROMFILE);

	m_bmpGDI.Attach(m_hBMP);

	//
	// Set the size of scroll view
	//
	size.cx = m_nBMPWidth;
	size.cy = m_nBMPHeight;

	size.cx = size.cx*m_nZoomPercent/100;
	size.cy = size.cy*m_nZoomPercent/100;
	SetScrollSizes(MM_TEXT, size);

	return 0;
}

void CImageAnalyzerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 計算此檢視的總大小
	sizeTotal.cx = 640;
	sizeTotal.cy = 480;
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
	TCHAR       szFileFilters_JPG[] = _T("Pictures (*.jpg)|*.jpg;*.jpeg|Pictures (*.bmp)|*.bmp||");
	TCHAR       szFileFilters_RAW[] = _T("RAW Data|*.raw||");
	CSize       scrollSize;
	int         i, j;
	int         pic_type = 0;
	TCHAR       szTempPath[MAX_PATH + 1];

	if (m_nAction == Action_MeasuringCalibrationData) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileFilters_RAW);
		if (IDOK != dlg.DoModal()) {
			return;
		}
		m_strInputFileName = dlg.GetPathName();
		m_strILL_Name[m_nCalibrationCnt] = m_strInputFileName;
	}
	else if (m_nAction == Action_MeasuringTeacherData) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileFilters_RAW);
		if (IDOK != dlg.DoModal()) {
			return;
		}
		m_strInputFileName = dlg.GetPathName();

		/* 
		//
		// The follwoing code transforms all the RAW data in a directory to BMP
		//
		CString tmp;
		for (i = m_strInputFileName.GetLength(); i >= 0; i--) {
			if (m_strInputFileName[i] == '\\') break;
		}
		j = i + 1;
		tmp.GetBufferSetLength(j);
		for (i = 0; i < j; i++) {
			tmp.GetBuffer()[i] = m_strInputFileName[i];
		}
		tmp.GetBuffer()[i] = '\0';
		RAW_to_BMP_Dir(tmp);
		*/


	}
	else if(m_nAction == Action_MeasuringRGB) {
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
		JPG_to_BMP(m_strInputFileName, m_strBMPFileName);
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
		RAW_to_BMP(m_strInputFileName, m_strBMPFileName);
		pic_type = 3;
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
			LoadBMPFile(m_strBMPFileName);

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

			break;
	}

	Invalidate();

	//if (m_nAction == Action_MeasuringCalibrationData) {
	//	MessageBox(L"Please select MCC vertices.");
	//}
}

int CImageAnalyzerView::MeasureCalibrationData_FileOpen()
{
	TCHAR       szFileFilters_RAW[] = _T("RAW Data|*.raw||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileFilters_RAW);

	if (IDOK != dlg.DoModal()) {
		if (1) {
			CStdioFile log_file;
			int        i, j;

			if (!log_file.Open(L"cal-log.txt", CStdioFile::modeWrite | CStdioFile::typeText))
			{
				if (!log_file.Open(L"cal-log.txt", CStdioFile::modeCreate | CStdioFile::modeWrite | CStdioFile::typeText))
				{
					MessageBox(L"Failed to create log file");
					return 0;
				}
			}
			for (i = 0; i < m_nCalibrationCnt + 1; i++) {
				log_file.WriteString(m_strILL_Name[i] + L"\n");
				for (j = 0; j < 4; j++) {
					CString tmp;
					tmp.Format(L"(%d, %d)", m_MCC_Vertices[i][j].x, m_MCC_Vertices[i][j].y);
					log_file.WriteString(tmp);
				}
				log_file.WriteString(L"\n");
			}
			log_file.Close();
		}
		return 0;
	}

	m_nCalibrationCnt++;
	m_nVertex = 0;
	m_strInputFileName = dlg.GetPathName();
	m_strILL_Name[m_nCalibrationCnt] = m_strInputFileName;

	RAW_to_BMP(m_strInputFileName, m_strBMPFileName);
	LoadBMPFile(m_strBMPFileName);
	Invalidate();

	return 0;
}

int CImageAnalyzerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此加入特別建立的程式碼

	return 0;
}

void CImageAnalyzerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼
	m_nViewWidth = cx;
	m_nViewHeight = cy;
}


void CImageAnalyzerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
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
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	m_bGotButtonDown = TRUE;
	m_MeasurePoint1 = point;

	CScrollView::OnLButtonDown(nFlags, point);
}


void CImageAnalyzerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (!m_bGotButtonDown) {
		return;
	}
	m_bGotButtonDown = FALSE;
	Invalidate();

	if (m_nAction == Action_MeasuringRGB) {
		MeasureRGB(point);
	} else if (m_nAction == Action_MeasuringCalibrationData) {
		if (m_nVertex < 4) {
			MeasureCalibrationData(point);
		} else if ( (m_nCalR[m_nCalibrationCnt]==0) && 
			        (m_nCalG[m_nCalibrationCnt]==0) && 
			        (m_nCalB[m_nCalibrationCnt]==0) ) 
		{
			MeasureWBGain(point);
		}
	}

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
				strInputFileName.GetBuffer()[i] = '/';
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
			nLight = (int)pow((m_R[i] * m_R[i] * 0.299) + (m_G[i] * m_G[i] * 0.587) + (m_B[i] * m_B[i] * 0.114), 0.5);
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

int CImageAnalyzerView::JPG_to_BMP(CString strFileName, CString strBMPFileName)
{
	//CString strBMPFileName(_T(""));
	CString strCmd;
	char    szCmd[1024];
	int     i;

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

int CImageAnalyzerView::RAW_to_BMP(CString strFileName, CString strBMPFileName, int nWidth, int nHeight, int nFormat)
{
	CFile         file;
	Img_RAW       raw;
	BMP           bmp;
	CRawFormatDlg dlg;
	int           format = -1, width = 0, height = 0;
	CString       strOutputFilename;
	int           i;

	file.Open(strFileName, CFile::modeRead | CFile::typeBinary);
	raw.SetBufferSize((unsigned int)file.GetLength());
	file.Read(raw.GetBuffer(), (unsigned int)file.GetLength());
	file.Close();

	if (nFormat == -1) {
		dlg.setInitState(m_nRAWDlg_width, m_nRAWDlg_height, m_nRAWDlg_format);
		if (IDOK == dlg.DoModal()) {
			format = dlg.GetFormat();
			dlg.GetWidthHeight(&width, &height);
			m_nRAWDlg_width = width;
			m_nRAWDlg_height = height;
			m_nRAWDlg_format = format;
		}
		else {
			return -1;
		}
	}
	else {
		width = nWidth;
		height = nHeight;
		format = nFormat;
	}

	raw.SetFormat(format, width, height);

	//
	// The following code transforms packed raw data to the unpacked.
	//
	/*
	if ((format == bayer_grbg_10bit_packed) || (format == bayer_gbrg_10bit_packed)) {
		CString strRawFileName = strFileName;
		CFile   newRawFile;

		strRawFileName += L"_";
		if (!newRawFile.Open(strRawFileName, CFile::modeWrite | CFile::typeBinary))
		{
			newRawFile.Open(strRawFileName, CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
		}
		newRawFile.Write(raw.GetUnpackedBuffer(), raw.GetUnpackedBufferSize());
	}
	*/

	bmp.SetBufferSize(width, height);
	for (i = 0; i < height; i++) {
		bmp.SetLine(raw.GetRGB()+i*width*3, i);
	}

	if (!file.Open(strBMPFileName, CFile::modeWrite | CFile::typeBinary))
	{
		file.Open(strBMPFileName, CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
	}
	file.Write(bmp.GetBuffer(), bmp.GetBufferSize());
	file.Close();

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
		int nOK;
		nOK = file.Open((LPCTSTR)strFilename,
			CStdioFile::modeReadWrite | CStdioFile::typeText | CStdioFile::modeCreate);
		if (!nOK) MessageBox(strFilename, L"Failed to open");
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

int CImageAnalyzerView::setValue(CString strFilename, CString strTag, int nValue)
{
	CString    buf;
	CStdioFile file;
	CString    strNewContent;
	int        i, j;

	if (!file.Open((LPCTSTR)strFilename, CStdioFile::modeReadWrite | CStdioFile::typeText)) {
		if (!file.Open( (LPCTSTR)strFilename,
						CStdioFile::modeReadWrite | CStdioFile::typeText | CStdioFile::modeCreate))
		{
			return -1;
		}
	}

	file.SeekToBegin();
	buf.GetBufferSetLength(128);
	do {
		if (!file.ReadString(buf.GetBuffer(), 128)) { // End of File
			buf.Empty();
			buf.Format(L"%s:%d\n", strTag, nValue);
			strNewContent += buf;
			buf.Empty();
			buf.GetBufferSetLength(128);
			break;
		}
		i = buf.Find(strTag);
		if (i == -1) {
			strNewContent += buf;
			continue;
		}
		j = buf.Find(L":", i);
		if (j == -1) {
			strNewContent += buf;
			continue;
		}
	} while (1);

	file.SeekToBegin();
	file.WriteString(strNewContent);
	file.Close();

	return 0;
}


void CImageAnalyzerView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nChar == 'S') {
		//
		// Save last time result
		//
		if (m_nAction == Action_MeasuringRGB) {
			SaveRGB();
		}
		else if (m_nAction == Action_MeasuringCalibrationData) {
			SaveCalibration();
		}
		else if (m_nAction == Action_MeasuringTeacherData) {
			CString strJPGFile = m_strInputFileName;
			int     i = strJPGFile.GetLength();
			strJPGFile.GetBuffer()[i-3] = 'j';
			strJPGFile.GetBuffer()[i-2] = 'p';
			strJPGFile.GetBuffer()[i-1] = 'g';
			SaveBMPFile();
			GenerateTeacher(strJPGFile);
		}
	} else if (nChar == 'R') {
		// Re-measure
		if (m_nAction == Action_MeasuringRGB) {
			ReMeasureRGB();
		}
		else if (m_nAction == Action_MeasuringCalibrationData) {
			ReMeasureCalibrationData();
		}
		Invalidate();
	}
	else if (nChar == 'N') {
		if (m_nAction == Action_MeasuringCalibrationData) {
			MeasureCalibrationData_FileOpen();
		}
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
		cmd.Format(L"move %s %s\\M\\", m_strInputFileName, dir);
		_wsystem(cmd);
	}

	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

int CImageAnalyzerView::SaveRGB()
{
	if (m_strResultFile.GetLength()) {
		SaveResult(m_strResultFile);
		m_strResultFile.Empty();
	}
	MessageBox(L"The Result has been saved.", L"OK", 0);

	return 0;
}

int CImageAnalyzerView::SaveCalibration()
{
	CStdioFile mmFile;
	CString    strPWD;
	CString    strLine;
	int        i, j, k, l;
	int        nOpenMode = CStdioFile::modeWrite | CStdioFile::typeText;

	for (i = m_strInputFileName.GetLength(); i >= 0; i--) {
		if (m_strInputFileName[i] == '\\') break;
	}
	l = i + 1;
	strPWD.GetBufferSetLength(l);
	for (i = 0; i < l; i++) {
		strPWD.GetBuffer()[i] = m_strInputFileName[i];
	}

	if (!mmFile.Open(strPWD + L"\\MakeMatrix.bat", nOpenMode)) {
		if (!mmFile.Open(strPWD + L"\\MakeMatrix.bat", CStdioFile::modeCreate | nOpenMode)) {
			return -1;
		}
	}

	strLine.Format(L"rem [edit] the definition of illuminants\n");
	mmFile.WriteString(strLine);
	for (i = 0; i < m_nCalibrationCnt + 1; i++) {
		CString strILL;
		strILL.Empty();
		strILL.GetBufferSetLength(m_strILL_Name[i].GetLength());
		for (j = m_strILL_Name[i].GetLength(); j >= 0; j--) {
			if (m_strILL_Name[i].GetBuffer()[j] == '\\') {
				j++;
				break;
			}
		}
		for (k = 0; j < m_strILL_Name[i].GetLength(); j++) {
			if (m_strILL_Name[i].GetAt(j) == '.') {
				break;
			}
			strILL.GetBuffer()[k] = m_strILL_Name[i].GetAt(j);
			k++;
		}
		strILL.GetBuffer()[k] = '\0';
		strLine.Format(L"set ILL%03d=%s\n", i+1, strILL);
		mmFile.WriteString(strLine);
	}

	mmFile.WriteString(L"\n");
	mmFile.WriteString(L"\n");
	mmFile.WriteString(L"set CHARTIMAGE_FOLDER=forMakeMatrix\\%TARGET%\\%CAMERA%_%SERIAL%\n");
	mmFile.WriteString(L"rem [edit] raw image file path of Macbeth color chart (input files)\n");
	for (i = 0; i < m_nCalibrationCnt + 1; i++) {
		CString strILL_RAW;
		strILL_RAW.Empty();
		strILL_RAW.GetBufferSetLength(m_strILL_Name[i].GetLength());
		for (j = m_strILL_Name[i].GetLength(); j >= 0; j--) {
			if (m_strILL_Name[i].GetBuffer()[j] == '\\') {
				j++;
				break;
			}
		}
		for (k = 0; j < m_strILL_Name[i].GetLength(); j++) {
			strILL_RAW.GetBuffer()[k] = m_strILL_Name[i].GetAt(j);
			k++;
		}
		strILL_RAW.GetBuffer()[k] = '\0';
		strLine.Format(L"set CHARTIMAGE_ILL%03d=%%CHARTIMAGE_FOLDER%%\\%s\n", i+1, strILL_RAW);
		mmFile.WriteString(strLine);
	}

	mmFile.WriteString(L"\n");
	mmFile.WriteString(L"\n");
	mmFile.WriteString(L"rem[edit] scanning area of Macbeth color chart(-scan option)\n");
	mmFile.WriteString(L"rem\n");
	mmFile.WriteString(L"rem v1(x1, y1)   v2(x2, y2)\n");
	mmFile.WriteString(L"rem     -------------\n");
	mmFile.WriteString(L"rem     |  Macbeth  |\n");
	mmFile.WriteString(L"rem     |   area    |\n");
	mmFile.WriteString(L"rem     -------------\n");
	mmFile.WriteString(L"rem v4(x4,y4)  v3(x3,y3)\n");
	mmFile.WriteString(L"rem\n");
	mmFile.WriteString(L"rem CHARTAREA_SAMPLE = x1, y1, x2, y2, x3, y3, x4, y4\n");
	for (i = 0; i < m_nCalibrationCnt + 1; i++) {
		strLine.Format(L"set CHARTAREA_ILL%03d=%d,%d,%d,%d,%d,%d,%d,%d\n",
			i+1,
			m_MCC_Vertices[i][0].x,
			m_MCC_Vertices[i][0].y,
			m_MCC_Vertices[i][1].x,
			m_MCC_Vertices[i][1].y,
			m_MCC_Vertices[i][2].x,
			m_MCC_Vertices[i][2].y,
			m_MCC_Vertices[i][3].x,
			m_MCC_Vertices[i][3].y
			);
		mmFile.WriteString(strLine);
	}

	mmFile.Close();
	return 0;
}

void CImageAnalyzerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CImageAnalyzerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
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
	// TODO: 在此加入您的命令處理常式程式碼
	m_nAction = Action_MeasuringTeacherData;

	return;
}

void CImageAnalyzerView::OnUpdateActionTeacherdata(CCmdUI *pCmdUI)
{
	// TODO: 在此加入您的命令更新 UI 處理常式程式碼
	if (m_nAction == Action_MeasuringTeacherData) {
		pCmdUI->SetCheck(1);
	}
	else {
		pCmdUI->SetCheck(0);
	}
}

void CImageAnalyzerView::OnActionMeasure()
{
	// TODO: 在此加入您的命令處理常式程式碼
	m_nAction = Action_MeasuringRGB;

}

void CImageAnalyzerView::OnUpdateActionMeasure(CCmdUI *pCmdUI)
{
	// TODO: 在此加入您的命令更新 UI 處理常式程式碼
	if (m_nAction == Action_MeasuringRGB) {
		pCmdUI->SetCheck(1);
	}
	else {
		pCmdUI->SetCheck(0);
	}
}

void CImageAnalyzerView::OnActionCalibration()
{
	// TODO: 在此加入您的命令處理常式程式碼
	m_nAction = Action_MeasuringCalibrationData;
}

void CImageAnalyzerView::OnUpdateActionCalibration(CCmdUI *pCmdUI)
{
	// TODO: 在此加入您的命令更新 UI 處理常式程式碼
	pCmdUI->SetCheck(m_nAction == Action_MeasuringCalibrationData);
}


void CImageAnalyzerView::OnZoomIn()
{
	// TODO: 在此加入您的命令處理常式程式碼
	CSize size;

	m_nZoomPercent += 5;
	size.cx = m_nBMPWidth * m_nZoomPercent / 100;
	size.cy = m_nBMPHeight * m_nZoomPercent / 100;
	SetScrollSizes(MM_TEXT, size);

	Invalidate();
}


void CImageAnalyzerView::OnZoomOut()
{
	// TODO: 在此加入您的命令處理常式程式碼
	CSize size;

	m_nZoomPercent -= 5;
	if (m_nZoomPercent < 5) {
		m_nZoomPercent = 5;
	}
	size.cx = m_nBMPWidth * m_nZoomPercent / 100;
	size.cy = m_nBMPHeight * m_nZoomPercent / 100;
	SetScrollSizes(MM_TEXT, size);

	Invalidate();
}

int CImageAnalyzerView::ReMeasureRGB()
{
	int i;

	m_MeasureCnt = 0;
	
	for (i = 0; i < 6; i++) {
		m_R[i] = 0;
		m_G[i] = 0;
		m_B[i] = 0;
	}
	return 0;
}

int CImageAnalyzerView::ReMeasureCalibrationData()
{
	int i;

	m_nVertex = 0;

	/*
	for (i = 0; i < 6; i++) {
		m_MCC_Vertices[i] = 0;
	}
	*/

	MessageBox(L"Please select MCC vertices.");

	return 0;
}

int CImageAnalyzerView::MeasureRGB(CPoint point)
{
	int    nWidth, nHeight;
	CPoint ScrollPoint;

	//
	//  m_MeasurePoint1 <- Passed in on LButtonDown
	//          +--------+
	//          |        |
	//          |        |
	//          +--------+
	//                   m_MeasurePoint2 <- Passed in on LButtonUp
	//
	if (m_MeasurePoint1 == point) {
		return 0;
	}
	m_MeasurePoint2 = point;

	nWidth  = (m_MeasurePoint2.x - m_MeasurePoint1.x)*100/m_nZoomPercent;
	nHeight = (m_MeasurePoint2.y - m_MeasurePoint1.y)*100/m_nZoomPercent;

	ScrollPoint = GetScrollPosition();
	m_nMeasureX = (ScrollPoint.x + m_MeasurePoint1.x)*100/m_nZoomPercent;
	m_nMeasureY = (ScrollPoint.y + m_MeasurePoint1.y)*100/m_nZoomPercent;

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

		m_L[m_MeasureCnt] = (float)pow((m_R[m_MeasureCnt] * m_R[m_MeasureCnt] * 0.299) +
			(m_G[m_MeasureCnt] * m_G[m_MeasureCnt] * 0.587) +
			(m_B[m_MeasureCnt] * m_B[m_MeasureCnt] * 0.114),
			0.5);
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
	
	return 0;
}

int CImageAnalyzerView::MeasureWBGain(CPoint point)
{
	int    nWidth, nHeight;
	CPoint ScrollPoint;

	//
	//  m_MeasurePoint1 <- Passed in on LButtonDown
	//          +--------+
	//          |        |
	//          |        |
	//          +--------+
	//                   m_MeasurePoint2 <- Passed in on LButtonUp
	//
	if (m_MeasurePoint1 == point) {
		return 0;
	}
	m_MeasurePoint2 = point;

	ScrollPoint = GetScrollPosition();
	m_ptCal[m_nCalibrationCnt][0].x = (ScrollPoint.x + m_MeasurePoint1.x)*100/m_nZoomPercent;
	m_ptCal[m_nCalibrationCnt][0].y = (ScrollPoint.y + m_MeasurePoint1.y)*100/m_nZoomPercent;
	m_ptCal[m_nCalibrationCnt][1].x = (ScrollPoint.x + m_MeasurePoint2.x)*100/m_nZoomPercent;
	m_ptCal[m_nCalibrationCnt][1].y = (ScrollPoint.y + m_MeasurePoint2.y)*100/m_nZoomPercent;

	nWidth  = m_ptCal[m_nCalibrationCnt][1].x - m_ptCal[m_nCalibrationCnt][0].x;
	nHeight = m_ptCal[m_nCalibrationCnt][1].y - m_ptCal[m_nCalibrationCnt][0].y;

	m_nAVRGr = m_bmp.GetAVG_R(m_ptCal[m_nCalibrationCnt][0].x, m_ptCal[m_nCalibrationCnt][0].y, nWidth, nHeight);
	m_nAVRGg = m_bmp.GetAVG_G(m_ptCal[m_nCalibrationCnt][0].x, m_ptCal[m_nCalibrationCnt][0].y, nWidth, nHeight);
	m_nAVRGb = m_bmp.GetAVG_B(m_ptCal[m_nCalibrationCnt][0].x, m_ptCal[m_nCalibrationCnt][0].y, nWidth, nHeight);

	m_nCalR[m_nCalibrationCnt] = m_nAVRGr/1000;
	m_nCalG[m_nCalibrationCnt] = m_nAVRGg/1000;
	m_nCalB[m_nCalibrationCnt] = m_nAVRGb/1000;

	return 0;
}

int CImageAnalyzerView::MeasureCalibrationData(CPoint point)
{
	CPoint scrollPosition = GetScrollPosition();

	if (m_nVertex < 4) {
		int i;
		m_MCC_Vertices[m_nCalibrationCnt][m_nVertex].x = (point.x + scrollPosition.x)*100/m_nZoomPercent;
		m_MCC_Vertices[m_nCalibrationCnt][m_nVertex].y = (point.y + scrollPosition.y)*100/m_nZoomPercent;
		m_nVertex++;
	}

	return 0;
}

int CImageAnalyzerView::SaveBMPFile()
{
	CString strCmd;
	CString strPWD;
	CString strBMPFileName;
	int     i, j, l;

	for (i = m_strInputFileName.GetLength(); i >= 0; i--) {
		if (m_strInputFileName[i] == '\\') break;
	}
	l = i + 1;
	strPWD.GetBufferSetLength(l);
	for (j = 0; j < i; j++) {
		strPWD.GetBuffer()[j] = m_strInputFileName[j];
	}
	strPWD.GetBuffer()[j] = '\0';

	strBMPFileName = m_strInputFileName;
	i = strBMPFileName.GetLength();
	strBMPFileName.GetBuffer()[i - 3] = 'b';
	strBMPFileName.GetBuffer()[i - 2] = 'm';
	strBMPFileName.GetBuffer()[i - 1] = 'p';

	strCmd += L"copy \"";
	strCmd += m_strBMPFileName;
	strCmd += L"\" \"";
	strCmd += strBMPFileName;
	strCmd += L"\"";

	_wsystem(strCmd);

	return 0;
}

int CImageAnalyzerView::RAW_to_BMP_Dir(CString strDirName)
{
	CFileFind finder;
	CString   strRAWFilePath;
	CString   strBMPFilePath;
	BOOL      bWorking = FALSE;
	int       i;

	bWorking = finder.FindFile(strDirName + L"\\*.raw");

	while (bWorking) {
		bWorking = finder.FindNextFile();

		if (finder.IsDots()) {
			continue;
		}
		if (finder.IsDirectory()) {
			continue;
		}
		strRAWFilePath = finder.GetFilePath();
		strBMPFilePath = strRAWFilePath;
		i = strBMPFilePath.GetLength();
		strBMPFilePath.GetBuffer()[i - 3] = 'b';
		strBMPFilePath.GetBuffer()[i - 2] = 'm';
		strBMPFilePath.GetBuffer()[i - 1] = 'p';

		RAW_to_BMP(strRAWFilePath, strBMPFilePath, 4032, 3024, 0);

	}
	return 0;
}

void CImageAnalyzerView::OnFileSaveAs()
{
	// TODO: 在此加入您的命令處理常式程式碼
	TCHAR       szFileFilters_RAW[] = _T("RAW Data (*.raw)|*.raw||");
	CString     strFileName;
	int         i, j;

	j = m_strInputFileName.GetLength();
	if ( ((m_strInputFileName.GetAt(j-3)&0xDF) == 'B') &&
		 ((m_strInputFileName.GetAt(j-2)&0xDF) == 'M') &&
		 ((m_strInputFileName.GetAt(j-1)&0xDF) == 'P')    )
	{
		// Get file name (without directory)
		for (i = j; i >= 0; i--)
		{
			if (m_strInputFileName[i] == '\\') break;
		}

		i = i + 1;
		while (i < j) {
			TCHAR ctemp = m_strInputFileName[i];
			if (ctemp == '.') {
				break;
			}
			strFileName += ctemp;
			i++;
		}
		strFileName += '\0';

		{
			CFileDialog dlg(FALSE, L"raw", strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileFilters_RAW);;

			if (IDOK == dlg.DoModal()) {
				strFileName = dlg.GetPathName();
				SaveBmpAsRaw(m_strInputFileName, strFileName);
			}
		}
	}
	else if ( ((m_strInputFileName.GetAt(j - 3) & 0xDF) == 'R') &&
		      ((m_strInputFileName.GetAt(j - 2) & 0xDF) == 'A') &&
		      ((m_strInputFileName.GetAt(j - 1) & 0xDF) == 'W') &&
		      ((m_nRAWDlg_format == bayer_grbg_10bit_packed)|| 
		       (m_nRAWDlg_format == bayer_gbrg_10bit_packed)  )

			)
	{
		//
		// Get file name (without directory)
		//
		for (i = j; i >= 0; i--)
		{
			if (m_strInputFileName[i] == '\\') break;
		}

		i = i + 1;
		while (i < j) {
			TCHAR ctemp = m_strInputFileName[i];
			if (ctemp == '.') {
				break;
			}
			strFileName += ctemp;
			i++;
		}
		strFileName += L"_unpacked";
		strFileName += '\0';

		{
			CFileDialog dlg(FALSE, L"raw", strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileFilters_RAW);
			CString     strUnpackedRawFileName;
			CFile       newRawFile;
			CFile       file;
			Img_RAW     raw;

			if (IDOK == dlg.DoModal()) {
				strUnpackedRawFileName = dlg.GetPathName();

				file.Open(m_strInputFileName, CFile::modeRead | CFile::typeBinary);
				raw.SetBufferSize((unsigned int)file.GetLength());
				file.Read(raw.GetBuffer(), (unsigned int)file.GetLength());
				file.Close();

				if (!newRawFile.Open(strUnpackedRawFileName, CFile::modeWrite | CFile::typeBinary))
				{
					newRawFile.Open(strUnpackedRawFileName, CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
				}
				raw.SetFormat(m_nRAWDlg_format, m_nRAWDlg_width, m_nRAWDlg_height);
				newRawFile.Write(raw.GetUnpackedBuffer(), raw.GetUnpackedBufferSize());
				newRawFile.Close();
			}
		}
	}
}

void CImageAnalyzerView::SaveBmpAsRaw(CString strBMPFileName, CString strRAWFileName)
{
	CFile   file_bmp;
	CFile   file_raw;
	BMP     bmp_src;
	Img_RAW raw_dst;
	int     nBMPWidth, nBMPHeight;
	int     i;

	file_bmp.Open(strBMPFileName, CFile::modeRead | CFile::typeBinary);
	bmp_src.SetBufferSize(file_bmp.GetLength());
	file_bmp.Read(bmp_src.GetBuffer(), bmp_src.GetBufferSize());
	bmp_src.ParseData();

	nBMPWidth = bmp_src.GetWidth();
	nBMPHeight = bmp_src.GetHeight();

	raw_dst.SetBufferSize(8 * ((nBMPWidth*nBMPHeight + 7) / 8) * 2);
	raw_dst.SetFormat(bayer_grbg_10bit_unpacked, nBMPWidth, nBMPHeight);

	for (i = 0; i < nBMPHeight; i++)
	{
		unsigned char *ptrLine;

		ptrLine = bmp_src.GetLine(i);
		raw_dst.SetLineBGR(ptrLine, i);

	}
	raw_dst.RGBtoRAW(bayer_grbg_10bit_unpacked);

	if (!file_raw.Open(strRAWFileName, CFile::modeWrite | CFile::typeBinary))
	{
		int r;
		r = file_raw.Open(strRAWFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}
	file_raw.Write(raw_dst.GetUnpackedBuffer(), raw_dst.GetUnpackedBufferSize());
	file_raw.Close();
}

int CImageAnalyzerView::GenerateTeacher(CString strJPGFile)
{
	CStdioFile file;
	CString    strLogFile;
	CString    strWBGainFile;
	CString    str;
	WCHAR      szBuf[1024];
	float      fAV, fTV, fSV;
	int        i, j, k;

	// DOS Command
	// exiftool -ExposureTime -ISO -FNUMBER -CREATEDATE img001.jpg   > exif.txt
	//                                                  [JPGFileName]
	str.Format(L".\\exiftool -ExposureTime -ISO -FNUMBER -CREATEDATE ");
	str += L'\"';
	str += strJPGFile;
	str += L'\"';
	str += L" > exif.txt";

	if ((j = str.GetLength()) > 1024) {
		return -1;
	}
	_wsystem(str);

	//
	// Get the values
	//

	// Exposure Time                   : 1/60
	//                                     ^^
	str = getValue(L"exif.txt", L"Exposure Time"); 
	j = str.GetLength();
	for (i = 0; i < j; i++) {
		if (str[i] == '/') {
			break;
		}
	}
	i++;
	k = 0;
	while (i < j) {
		szBuf[k++] = str[i++];
		if ((str[i] == ' ') || (str[i] == '\n') || (str[i] == '\0')) {
			break;
		}
	}
	szBuf[k] = '\0';
	m_nExposure = _ttoi(szBuf);

	// ISO                             : 1600
	//                                   ^^^^
	str = getValue(_T("exif.txt"), _T("ISO"));
	j = str.GetLength();
	for (i = 0; i < j; i++) {
		szBuf[i] = str[i];
		if ((str[i] == ' ') || (str[i] == '\n') || (str[i] == '\0')) {
			break;
		}
	}
	szBuf[i] = '\0';
	m_nISO = _ttoi(szBuf);

    //F Number                        : 3.5
	//                                  ^^^
	str = getValue(L"exif.txt", L"F Number");
	j = str.GetLength();
	for (i = 0; i < j; i++) {
		szBuf[i] = str[i];
		if (str[i] == '.') {
			break;
		}
	}
	szBuf[i++] = '\0';
	m_nFnum_n = _ttoi(szBuf);
	for (k = 0; i < j; i++, k++) {
		szBuf[k] = str[i];
		if ((str[i] == ' ') || (str[i] == '\n') || (str[i] == '\0')) {
			break;
		}
	}
	szBuf[k] = '\0';
	m_nFnum_f = _ttoi(szBuf);

	//
	// LOG file
	//
	str.Empty();
	strLogFile = strJPGFile;
	i = strLogFile.GetLength();
	strLogFile.GetBuffer()[i-3] = 'l';
	strLogFile.GetBuffer()[i-2] = 'o';
	strLogFile.GetBuffer()[i-1] = 'g';

	CFile::Remove(strLogFile);
	file.Open(strLogFile, CStdioFile::modeWrite | CStdioFile::typeText | CStdioFile::modeCreate);
	file.WriteString(L"[IMAGE_INFO]\n");
	file.WriteString(L"Endian=0\n");
	str.Format(L"Width=%d\n", m_nRAWDlg_width);
	file.WriteString(str);
	str.Format(L"Height=%d\n", m_nRAWDlg_height);
	file.WriteString(str);
	if ( (m_nRAWDlg_format == bayer_grbg_10bit_packed)   || 
		 (m_nRAWDlg_format == bayer_grbg_10bit_unpacked)   )
	{
		file.WriteString(L"TopLeftColor=1\n");
		file.WriteString(L"PixelMaxValue=1023\n");
	}
	else if( (m_nRAWDlg_format == bayer_gbrg_10bit_packed)  ||
		     (m_nRAWDlg_format == bayer_gbrg_10bit_unpacked)  )
	{
		file.WriteString(L"TopLeftColor=2\n");
		file.WriteString(L"PixelMaxValue=1023\n");
	}
	else {
		file.WriteString(L"TopLeftColor=\n");
		file.WriteString(L"PixelMaxValue=\n");
	}
	file.WriteString(L"Offset=0\n");
	file.WriteString(L"[SHOT_INFO]\n");
	
	str.Format(L"%d.%d\n", m_nFnum_n, m_nFnum_f);
	fAV = _ttof(str);
	fAV = 2 * log2(fAV);
	str.Format(L"Av=%.2f\n", fAV);
	file.WriteString(str);

	fTV = log2(m_nExposure*1.0);
	str.Format(L"Tv=%.2f\n", fTV);
	file.WriteString(str);

	fSV = log2(m_nISO / 3.0);
	str.Format(L"Sv=%.2f\n", fSV);
	file.WriteString(str);

	file.WriteString(L"BvFlash=100\n");
	file.WriteString(L"[TEACHER_INFO]\n");
	file.WriteString(L"CameraWBR=1.00\n");
	file.WriteString(L"CameraWBB=1.00\n");

	file.Close();

	return 0;
}