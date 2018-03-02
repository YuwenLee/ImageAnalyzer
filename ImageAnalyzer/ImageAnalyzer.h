
// ImageAnalyzer.h : ImageAnalyzer 應用程式的主標頭檔
//
#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"       // 主要符號


// CImageAnalyzerApp:
// 請參閱實作此類別的 ImageAnalyzer.cpp
//

class CImageAnalyzerApp : public CWinApp
{
public:
	CImageAnalyzerApp();


// 覆寫
public:
	virtual BOOL InitInstance();

// 程式碼實作
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CImageAnalyzerApp theApp;
