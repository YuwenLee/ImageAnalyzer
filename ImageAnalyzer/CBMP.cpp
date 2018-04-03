#include "stdafx.h"
#include "CBMP.h"

#pragma pack(push)
#pragma pack(1)
typedef struct                        /**** BMP file header structure ****/
{
	unsigned short bfType;           /* Magic number for file */
	unsigned int   bfSize;           /* Size of file */
	unsigned short bfReserved1;      /* Reserved */
	unsigned short bfReserved2;      /* ... */
	unsigned int   bfOffBits;        /* Offset to bitmap data */
} _BITMAPFILEHEADER_;

typedef struct                       /**** BMP file info structure ****/
{
	unsigned int   biSize;           /* Size of info header */
	int            biWidth;          /* Width of image */
	int            biHeight;         /* Height of image */
	unsigned short biPlanes;         /* Number of color planes */
	unsigned short biBitCount;       /* Number of bits per pixel */
	unsigned int   biCompression;    /* Type of compression to use */
	unsigned int   biSizeImage;      /* Size of image data */
	int            biXPelsPerMeter;  /* X resolution */
	int            biYPelsPerMeter;  /* Y resolution */
	unsigned int   biClrUsed;        /* Number of colors used */
	unsigned int   biClrImportant;   /* Number of important colors */
} _BITMAPINFOHEADER_;
#pragma pack(pop)

BMP::BMP()
	:m_pData(NULL), 
	 m_uSize(0)
{

}

BMP::~BMP()
{
	if (m_pData) free(m_pData);
	m_pData = NULL;
	m_uSize = 0;
}

int BMP::SetBufferSize(unsigned int uSize)
{
	_BITMAPFILEHEADER_ *bfh;

	if (m_pData) {
		free(m_pData);
		m_pData = NULL;
		m_uSize = 0;
	}

	m_pData = (unsigned char *)malloc(uSize);
	if (m_pData == NULL) {
		m_uSize = 0;
		return -1;
	}
	
	m_uSize = uSize;
	memset(m_pData, 0, m_uSize);

	bfh = (_BITMAPFILEHEADER_*)m_pData;

	// Assume Little Endian
	bfh->bfType = 0x4d42;
	bfh->bfSize = m_uSize; // total file size
	bfh->bfReserved1 = 0;
	bfh->bfReserved2 = 0;
	bfh->bfOffBits = sizeof(_BITMAPFILEHEADER_) + sizeof(_BITMAPINFOHEADER_); // headers' size;

	return 0;
}

int BMP::SetBufferSize(int nWidth, int nHeight)
{
	_BITMAPINFOHEADER_ *bih;
	unsigned int        uLineSize;
	unsigned int        uTotalSize;
	int                 nErr = -1;

	uLineSize = (((unsigned int)nWidth * 3 + 3) / 4) * 4; // aligned to 4 bytes
	uTotalSize = sizeof(_BITMAPFILEHEADER_) + sizeof(_BITMAPINFOHEADER_) + uLineSize*(unsigned int)nHeight;
	nErr = SetBufferSize(uTotalSize);
	if (nErr) {
		return nErr;
	}
	
	n_width = nWidth;
	n_height = nHeight;
	
	bih = (_BITMAPINFOHEADER_ *)GetInfoHeader();
	bih->biSize = sizeof(_BITMAPINFOHEADER_);
	bih->biBitCount = 24;
	bih->biClrImportant = 0;
	bih->biClrUsed = 0;
	bih->biCompression = 0;
	bih->biWidth = n_width;
	bih->biHeight = n_height;
	bih->biPlanes = 1;
	bih->biSizeImage = uLineSize*(unsigned int)nHeight;
	bih->biXPelsPerMeter = 0x0ec4;
	bih->biYPelsPerMeter = 0x0ec4;

	return nErr;
}

int BMP::GetWidth()
{
	_BITMAPINFOHEADER_ *pih;

	if (!m_pData) return -1;

	pih = (_BITMAPINFOHEADER_*)(m_pData + sizeof(_BITMAPFILEHEADER_));

	return pih->biWidth;
}

int BMP::GetHeight()
{
	_BITMAPINFOHEADER_ *pih;

	if (!m_pData) return -1;

	pih = (_BITMAPINFOHEADER_*)(m_pData + sizeof(_BITMAPFILEHEADER_));

	return pih->biHeight;
}

unsigned char * BMP::GetRGB()
{
	if (m_pData == NULL) {
		return NULL;
	}

	return (m_pData + sizeof(_BITMAPINFOHEADER_) + sizeof(_BITMAPFILEHEADER_));
}

unsigned char * BMP::GetFileHeader()
{
	if (m_pData == NULL) {
		return NULL;
	}

	return m_pData;
}

unsigned char * BMP::GetInfoHeader()
{
	if (m_pData == NULL) {
		return NULL;
	}

	return (m_pData + sizeof(_BITMAPFILEHEADER_));
}

int BMP::GetAVG_R(int x, int y, int w, int h)
{
	unsigned int   avrg;
	unsigned char *ptr;
	int            nImgWidth;
	int            nLinePadding;
	int            nLineBytes;
	int            i, j;

	avrg = 0;
	ptr = GetRGB();

	if (ptr == NULL) {
		return 0;
	}

	nImgWidth = GetWidth() * 3;
	nLinePadding = (4 - nImgWidth % 4) % 4;
 	nLineBytes = nImgWidth + nLinePadding;

	ptr += (GetHeight() - y)*nLineBytes;

	for (j = 0; j < h ; j++) {
		ptr -= nLineBytes;
		for (i = x; i < (x + w); i++) {
			avrg += *(ptr + i*3 + 2);
		}
	}
	avrg = avrg*1000 / (w*h);

	return avrg;
}

int BMP::GetAVG_G(int x, int y, int w, int h)
{
	unsigned int   avrg;
	unsigned char *ptr;
	int            nImgWidth;
	int            nLinePadding;
	int            nLineBytes;
	int            i, j;

	avrg = 0;
	ptr = GetRGB();

	if (ptr == NULL) {
		return 0;
	}

	nImgWidth = GetWidth() * 3;
	nLinePadding = (4 - nImgWidth % 4) % 4;
	nLineBytes = nImgWidth + nLinePadding;

	ptr += (GetHeight() - y)*nLineBytes;

	if (w == 0) w = 2;
	if (h == 0) h = 2;

	for (j = 0; j < h; j++) {
		ptr -= nLineBytes;
		for (i = x; i < (x + w); i++) {
			avrg += *(ptr + i*3 + 1);
		}
	}
	avrg = avrg * 1000 / (w*h);

	return avrg;
}

int BMP::GetAVG_B(int x, int y, int w, int h)
{
	unsigned int   avrg;
	unsigned char *ptr;
	int            nImgWidth;
	int            nLinePadding;
	int            nLineBytes;
	int            i, j;

	avrg = 0;
	ptr = GetRGB();

	if (ptr == NULL) {
		return 0;
	}

	nImgWidth = GetWidth() * 3;
	nLinePadding = (4 - nImgWidth % 4) % 4;
	nLineBytes = nImgWidth + nLinePadding;

	ptr += (GetHeight() - y)*nLineBytes;

	for (j = 0; j < h; j++) {
		ptr -= nLineBytes;
		for (i = x; i < (x + w); i++) {
			avrg += *(ptr + i*3);
		}
	}
	avrg = avrg * 1000 / (w*h);

	return avrg;
}

int BMP::SetLine(unsigned char *pRGB, int nLine)
{
	unsigned int   uX, y_src, y_dst;
	unsigned int   uLineSize, uDataSize;
	unsigned char *pDst = GetRGB();

	uDataSize = n_width * 3;
	uLineSize = ((uDataSize + 3) / 4) * 4;

	// Write the 1st source line to the last destination line
	y_dst = n_height - 1 - nLine;
	pDst += y_dst*uLineSize;
	for (uX = 0; uX < uDataSize; uX++) {
		pDst[uX] = pRGB[uX];
	}

	return 0;
}

unsigned char * BMP::GetBuffer()
{
	return m_pData;
}

unsigned int BMP::GetBufferSize()
{
	return m_uSize;
}