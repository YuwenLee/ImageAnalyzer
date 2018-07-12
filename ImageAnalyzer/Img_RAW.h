#ifndef _170313_IMG_RAW_
#define _170313_IMG_RAW_

#include "stdafx.h"

typedef enum {
	bayer_grbg_10bit_packed = 0,
	bayer_grbg_10bit_unpacked,
	bayer_gbrg_10bit_packed,
	bayer_gbrg_10bit_unpacked
} img_raw_format ;

class Img_RAW
{
private:
	int            m_nDebugRAW;
	int            m_nDebugBMP;
	static int     s_nBrightness; /* 24.8 fixed point */
	static int     s_nSwaprb;

	void writergb(void *addr, int bpp, unsigned char r, unsigned char g, unsigned char b);
	void writergb_10(void *addr, int bpp, unsigned short r, unsigned short g, unsigned short b);
	void bay2rgb_10(unsigned short *bay, int bay_line, unsigned char *rgb, int rgb_line, int columns, int rows, int bpp);
	int WriteToBMP(CString strFileName, int nOverwrite = 0);

protected:
    int            m_nWidth;
    int            m_nHeight;
    unsigned char *m_pData;
    unsigned char *m_pUnpackedData;
	unsigned char *m_pRGB;
    unsigned int   m_uSize;
    int            m_nFormat;

	int RGB_Interpolation(unsigned char *pRAW, int nWidth, int nHeight, unsigned char *pRGB);

public:
    Img_RAW();
    ~Img_RAW();
    int SetBufferSize(unsigned int uSize);
    int SetFormat(int nFormat, int nWidth, int nHeight);
	int SetLineBGR(unsigned char *pBGR, int nLine);
	int RGBtoRAW(int nRawFormat);
    int GetPixel(int x, int y);
    unsigned char * GetBuffer();
	unsigned char * GetUnpackedBuffer();
	unsigned char * GetRGB();
	unsigned int GetUnpackedBufferSize();
};

#endif //_170313_IMG_RAW_
