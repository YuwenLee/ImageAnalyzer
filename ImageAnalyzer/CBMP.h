#ifndef _170313_CBMP_
#define _170313_CBMP_

#include "stdafx.h"

class BMP
{
protected:
	int n_width;
	int n_height;
	unsigned char *m_pData;
	unsigned int   m_uSize;

public:
	BMP();
	~BMP();
	int SetBufferSize(unsigned int uSize);
	int GetWidth();
	int GetHeight();
	unsigned char * GetRGB();
	unsigned char * GetFileHeader();
	unsigned char * GetInfoHeader();
};

#endif //_170313_CBMP_