
// ImageAnalyzer.h : ImageAnalyzer ���ε{�����D���Y��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"       // �D�n�Ÿ�


// CImageAnalyzerApp:
// �аѾ\��@�����O�� ImageAnalyzer.cpp
//

class CImageAnalyzerApp : public CWinApp
{
public:
	CImageAnalyzerApp();


// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CImageAnalyzerApp theApp;
