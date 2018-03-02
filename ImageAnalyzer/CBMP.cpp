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
	if (m_pData) {
		free(m_pData);
		m_pData = NULL;
	}

	m_pData = (unsigned char *)malloc(uSize);
	if (m_pData == NULL) {
		return -1;
	}

	return 0;
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
	int            i, j, k;

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
	int            i, j, k;

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
	int            i, j, k;

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
