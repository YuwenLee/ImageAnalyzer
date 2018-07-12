#include "stdafx.h"
#include "Img_RAW.h"

#define DETECT_BADVAL 1

int Img_RAW::s_nBrightness = 256; /* 24.8 fixed point */
int Img_RAW::s_nSwaprb     = 0;

Img_RAW::Img_RAW()
	: m_pData(NULL)
	, m_pUnpackedData(NULL)
	, m_pRGB(NULL)
	, m_uSize(0)
{
	m_nDebugRAW = 0;
	m_nDebugBMP = 0;
}

Img_RAW::~Img_RAW()
{
	if (m_pData) free(m_pData);
	m_pData = NULL;

	if (m_pUnpackedData) free(m_pUnpackedData);
	m_pUnpackedData = NULL;

	if (m_pRGB) free(m_pRGB);
	m_pRGB = NULL;

	m_uSize = 0;
}


int Img_RAW::SetBufferSize(unsigned int uSize)
{
	if (m_pData) {
		free(m_pData);
		m_pData = NULL;
	}

	m_pData = (unsigned char *)malloc(uSize);
	if (m_pData == NULL) {
		return -1;
	}
	m_uSize = uSize;

	return 0;
}

int Img_RAW::SetFormat(int nFormat, int nWidth, int nHeight)
{
	int i, j, k;
	m_nFormat = nFormat;
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if (!m_pData) {
		return -1;
	}

	if (m_pUnpackedData) free(m_pUnpackedData);
	m_pUnpackedData = NULL;

	if (m_pRGB) {
		free(m_pRGB);
	}
	m_pRGB = NULL;
	
	if ((nFormat == bayer_grbg_10bit_packed) || (nFormat == bayer_gbrg_10bit_packed)) {

		m_uSize = 8 * ((m_nWidth*m_nHeight + 7) / 8) * 2; // Align to 8 bytes
		m_pUnpackedData = (unsigned char *)malloc(m_uSize * sizeof(unsigned int));
		memset(m_pUnpackedData, 0, m_uSize);

		i = 0;
		j = 0;
		k = 0;
		do {
			unsigned int uTmp;

			uTmp = ((m_pData[j + 1] & 0x03) << 8) | m_pData[j];
			m_pUnpackedData[k++] = uTmp & 0x000000FF;
			m_pUnpackedData[k++] = (uTmp & 0x0000FF00) >> 8;
			i++;
			j++;

			uTmp = ((m_pData[j + 1] & 0x0F) << 6) | (m_pData[j] >> 2);
			m_pUnpackedData[k++] = uTmp & 0x000000FF;
			m_pUnpackedData[k++] = (uTmp & 0x0000FF00) >> 8;
			i++;
			j++;

			uTmp = ((m_pData[j + 1] & 0x3F) << 4) | (m_pData[j] >> 4);
			m_pUnpackedData[k++] = uTmp & 0x000000FF;
			m_pUnpackedData[k++] = (uTmp & 0x0000FF00) >> 8;
			i++;
			j++;

			uTmp = (m_pData[j + 1] << 2) | (m_pData[j] >> 6);
			m_pUnpackedData[k++] = uTmp & 0x000000FF;
			m_pUnpackedData[k++] = (uTmp & 0x0000FF00) >> 8;
			i++;
			j += 2;

			uTmp = ((m_pData[j + 1] & 0x03) << 8) | m_pData[j];
			m_pUnpackedData[k++] = uTmp & 0x000000FF;
			m_pUnpackedData[k++] = (uTmp & 0x0000FF00) >> 8;
			i++;
			j++;

			uTmp = ((m_pData[j + 1] & 0x0F) << 6) | (m_pData[j] >> 2);
			m_pUnpackedData[k++] = uTmp & 0x000000FF;
			m_pUnpackedData[k++] = (uTmp & 0x0000FF00) >> 8;
			i++;
			j += 2;

		} while (i < m_nWidth*m_nHeight);
		free(m_pData);
		m_pData = NULL;
	} else if ((nFormat == bayer_grbg_10bit_unpacked)||(nFormat == bayer_gbrg_10bit_unpacked)) {
		m_pUnpackedData = m_pData;
		m_pData = NULL;
	}

	if(m_nDebugRAW) {
		FILE *fp = fopen("temp.raw", "wb");
		fwrite(m_pUnpackedData, m_uSize, 1, fp);
		fclose(fp);
	}
	
	m_pRGB = (unsigned char *)malloc(m_nWidth*m_nHeight * 3);
	RGB_Interpolation(m_pUnpackedData, m_nWidth, m_nHeight, m_pRGB);

	if(m_nDebugBMP) {
		WriteToBMP(L"temp.bmp", 1);
	}
	return 0;
}


int Img_RAW::GetPixel(int x, int y)
{
	return 0;
}


unsigned char * Img_RAW::GetBuffer()
{
	return m_pData;
}

unsigned char * Img_RAW::GetUnpackedBuffer()
{
	return m_pUnpackedData;
}

unsigned char * Img_RAW::GetRGB()
{
	return m_pRGB;
}

int Img_RAW::WriteToBMP(CString strFileName, int nOverwrite)
{
	CFile             file;
	BITMAPFILEHEADER  bfh;
	BITMAPINFOHEADER  bih;
	int               img_line_bytes, total_line_bytes;
	int               padding;
	unsigned long     headers_size;
	int               img_size_with_padding;
	unsigned char    *p_img_with_padding = NULL;
	int               nError = -1;

	if (nOverwrite == 0) {
		if (PathFileExists(strFileName)) {
			return nError;
		}
	}

	if (!file.Open(strFileName, CFile::modeReadWrite)) {
		// The file does not exist. Create it.
		if(!file.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite) )
		{
			return nError;
		}
	}
    
	headers_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	img_line_bytes = m_nWidth * 3;
	padding = (4 - img_line_bytes % 4) % 4;
	total_line_bytes = img_line_bytes + padding;
	img_size_with_padding = total_line_bytes*m_nHeight;

	p_img_with_padding = (unsigned char *)malloc(img_size_with_padding);
	if (p_img_with_padding == NULL) {
		return nError;
	}
	memset(p_img_with_padding, 0, img_size_with_padding);

	// Set BMP Info Header
	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biBitCount = 24;
	bih.biClrImportant = 0;
	bih.biClrUsed = 0;
	bih.biCompression = 0;
	bih.biWidth = m_nWidth;
	bih.biHeight = m_nHeight;
	bih.biPlanes = 1;
	bih.biSizeImage = img_size_with_padding;
	bih.biXPelsPerMeter = 0x0ec4;
	bih.biYPelsPerMeter = 0x0ec4;

	// Set BMP File Header
	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 0x4d42;
	bfh.bfSize = headers_size + img_size_with_padding;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = headers_size;

	// Write BMP File Header
	{
		unsigned char buf[4];

		buf[0] = (bfh.bfType & 0x00FF);
		buf[1] = (bfh.bfType & 0xFF00) >> 8;
		file.Write(buf, sizeof(bfh.bfType));

		buf[0] = (bfh.bfSize & 0x000000FF);
		buf[1] = (bfh.bfSize & 0x0000FF00) >> 8;
		buf[2] = (bfh.bfSize & 0x00FF0000) >> 16;
		buf[3] = (bfh.bfSize & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bfh.bfSize));

		buf[0] = (bfh.bfReserved1 & 0x00FF);
		buf[1] = (bfh.bfReserved1 & 0xFF00) >> 8;
		file.Write(buf, sizeof(bfh.bfReserved1));

		buf[0] = (bfh.bfReserved2 & 0x00FF);
		buf[1] = (bfh.bfReserved2 & 0xFF00) >> 8;
		file.Write(buf, sizeof(bfh.bfReserved2));

		buf[0] = (bfh.bfOffBits & 0x000000FF);
		buf[1] = (bfh.bfOffBits & 0x0000FF00) >> 8;
		buf[2] = (bfh.bfOffBits & 0x00FF0000) >> 16;
		buf[3] = (bfh.bfOffBits & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bfh.bfOffBits));

	}

	// Write BMP Info Header
	{
		unsigned char buf[4];

		buf[0] = (bih.biSize & 0x000000FF);
		buf[1] = (bih.biSize & 0x0000FF00) >> 8;
		buf[2] = (bih.biSize & 0x00FF0000) >> 16;
		buf[3] = (bih.biSize & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biSize));

		buf[0] = (bih.biWidth & 0x000000FF);
		buf[1] = (bih.biWidth & 0x0000FF00) >> 8;
		buf[2] = (bih.biWidth & 0x00FF0000) >> 16;
		buf[3] = (bih.biWidth & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biWidth));

		buf[0] = (bih.biHeight & 0x000000FF);
		buf[1] = (bih.biHeight & 0x0000FF00) >> 8;
		buf[2] = (bih.biHeight & 0x00FF0000) >> 16;
		buf[3] = (bih.biHeight & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biHeight));

		buf[0] = (bih.biPlanes & 0x000000FF);
		buf[1] = (bih.biPlanes & 0x0000FF00) >> 8;
		file.Write(buf, sizeof(bih.biPlanes));

		buf[0] = (bih.biBitCount & 0x000000FF);
		buf[1] = (bih.biBitCount & 0x0000FF00) >> 8;
		file.Write(buf, sizeof(bih.biBitCount));

		buf[0] = (bih.biCompression & 0x000000FF);
		buf[1] = (bih.biCompression & 0x0000FF00) >> 8;
		file.Write(buf, sizeof(bih.biCompression));

		buf[0] = (bih.biSizeImage & 0x000000FF);
		buf[1] = (bih.biSizeImage & 0x0000FF00) >> 8;
		buf[2] = (bih.biSizeImage & 0x00FF0000) >> 16;
		buf[3] = (bih.biSizeImage & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biSizeImage));

		buf[0] = (bih.biXPelsPerMeter & 0x000000FF);
		buf[1] = (bih.biXPelsPerMeter & 0x0000FF00) >> 8;
		buf[2] = (bih.biXPelsPerMeter & 0x00FF0000) >> 16;
		buf[3] = (bih.biXPelsPerMeter & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biXPelsPerMeter));

		buf[0] = (bih.biYPelsPerMeter & 0x000000FF);
		buf[1] = (bih.biYPelsPerMeter & 0x0000FF00) >> 8;
		buf[2] = (bih.biYPelsPerMeter & 0x00FF0000) >> 16;
		buf[3] = (bih.biYPelsPerMeter & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biYPelsPerMeter));

		buf[0] = (bih.biClrUsed & 0x000000FF);
		buf[1] = (bih.biClrUsed & 0x0000FF00) >> 8;
		buf[2] = (bih.biClrUsed & 0x00FF0000) >> 16;
		buf[3] = (bih.biClrUsed & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biClrUsed));

		buf[0] = (bih.biClrImportant & 0x000000FF);
		buf[1] = (bih.biClrImportant & 0x0000FF00) >> 8;
		buf[2] = (bih.biClrImportant & 0x00FF0000) >> 16;
		buf[3] = (bih.biClrImportant & 0xFF000000) >> 24;
		file.Write(buf, sizeof(bih.biClrImportant));

	}

	// Write Impage
	{
		int x, y_src, y_dst;

		for (y_src = 0, y_dst = m_nHeight - 1; y_src<m_nHeight; y_src++, y_dst--) {
			for (x = 0; x<img_line_bytes; x++) {
				p_img_with_padding[y_dst*total_line_bytes + x] = m_pRGB[y_src*img_line_bytes + x];
			}
		}

		file.Write(p_img_with_padding, img_size_with_padding);
	}

	if (p_img_with_padding) {
		free(p_img_with_padding);
		p_img_with_padding = NULL;
	}

	file.Close();
	nError = 0;

	return nError;
}

int Img_RAW::RGB_Interpolation(unsigned char *pRAW, int nWidth, int nHeight, unsigned char *pRGB)
{
	unsigned int   src_stride = nWidth*2;
	unsigned int   rgb_stride = nWidth*3;
	unsigned char *buf;
	int            dst_x, dst_y;
	int            shift = 0;
	unsigned char *ptr = pRAW;

	for (dst_y = 0; dst_y<nHeight; dst_y++) {
		for (dst_x = 0; dst_x<nWidth; dst_x++) {
			int v = ptr[0] | ptr[1] << 8;
			v *= s_nBrightness;
			v >>= 8;
			if (v < 0) v = 0;
			if (v >= (1 << 10)) v = (1 << 10) - 1;
			ptr[0] = v & 0x000000FF;
			ptr[1] = (v & 0x0000FF00) >> 8;

			ptr += 2;
		}
	}

	buf = (unsigned char *)malloc(nWidth*nHeight*3);
	bay2rgb_10((unsigned short *)pRAW, nWidth, buf, nWidth*3, nWidth, nHeight, 3);

	if ((m_nFormat == bayer_grbg_10bit_packed) || (m_nFormat == bayer_grbg_10bit_unpacked))
	{
		s_nSwaprb = 1;
	}
	else if ((m_nFormat == bayer_gbrg_10bit_packed) || (m_nFormat == bayer_gbrg_10bit_unpacked)) {
		s_nSwaprb = 0;
	}

	for (dst_y = 0; dst_y<nHeight; dst_y++) {
		for (dst_x = 0; dst_x<nWidth; dst_x++) {
			ptr = buf + nWidth * 3 * dst_y + dst_x * 3;
			pRGB[dst_y*rgb_stride + 3 * dst_x + 0] = s_nSwaprb ? ptr[2] : ptr[0];
			pRGB[dst_y*rgb_stride + 3 * dst_x + 1] = ptr[1]; 
			pRGB[dst_y*rgb_stride + 3 * dst_x + 2] = s_nSwaprb ? ptr[0] : ptr[2];
		}
	}
	free(buf);
	return 0;
}

inline void Img_RAW::writergb(
	void *addr, int bpp,
	unsigned char r, unsigned char g, unsigned char b)
{
	if (bpp == 4) {

		unsigned char *addr2 = (unsigned char *)addr;
		addr2[0] = r;
		addr2[1] = g;
		addr2[2] = b;
		addr2[3] = 0;
	}
	else {
		unsigned char *addr2 = (unsigned char *)addr;
		addr2[0] = r;
		addr2[1] = g;
		addr2[2] = b;
	}
}

inline void Img_RAW::writergb_10(
	void *addr, int bpp,
	unsigned short r, unsigned short g, unsigned short b)
{
	writergb(addr, bpp, r >> 2, g >> 2, b >> 2);
}

inline void Img_RAW::bay2rgb_10(
	unsigned short *bay, int bay_line,
	unsigned char *rgb, int rgb_line,
	int columns, int rows, int bpp)
{
	int             nError;
	unsigned short *cur_bay;
	unsigned char  *cur_rgb;
	int             bay_line2, rgb_line2;
	int             total_columns;

	/* Process 2 lines and rows per each iteration, but process the last row and column separately */
	total_columns = (columns >> 1) - 1;
	rows = (rows >> 1) - 1;
	bay_line2 = 2 * bay_line;
	rgb_line2 = 2 * rgb_line;
	do {
		cur_bay = bay;
		cur_rgb = rgb;
		columns = total_columns;
		do {
			writergb_10(cur_rgb + 0, bpp, cur_bay[1], cur_bay[0], cur_bay[bay_line]);
			writergb_10(cur_rgb + bpp, bpp, cur_bay[1], cur_bay[2], cur_bay[bay_line + 2]);
			writergb_10(cur_rgb + rgb_line, bpp, cur_bay[bay_line2 + 1], cur_bay[bay_line + 1], cur_bay[bay_line]);
			writergb_10(cur_rgb + rgb_line + bpp, bpp, cur_bay[bay_line2 + 1], cur_bay[bay_line + 1], cur_bay[bay_line + 2]);
			cur_bay += 2;
			cur_rgb += 2 * bpp;
		} while (--columns);
		writergb_10(cur_rgb + 0, bpp, cur_bay[1], cur_bay[0], cur_bay[bay_line]);
		writergb_10(cur_rgb + bpp, bpp, cur_bay[1], cur_bay[bay_line + 1], cur_bay[bay_line]);
		writergb_10(cur_rgb + rgb_line, bpp, cur_bay[bay_line2 + 1], cur_bay[bay_line + 1], cur_bay[bay_line]);
		writergb_10(cur_rgb + rgb_line + bpp, bpp, cur_bay[bay_line2 + 1], cur_bay[bay_line + 1], cur_bay[bay_line]);
		bay += bay_line2;
		rgb += rgb_line2;
	} while (--rows);
	
	cur_bay = bay;
	cur_rgb = rgb;
	columns = total_columns;
	do {
		writergb_10(cur_rgb + 0, bpp, cur_bay[1], cur_bay[0], cur_bay[bay_line]);
		writergb_10(cur_rgb + bpp, bpp, cur_bay[1], cur_bay[2], cur_bay[bay_line + 2]);
		writergb_10(cur_rgb + rgb_line, bpp, cur_bay[1], cur_bay[bay_line + 1], cur_bay[bay_line]);
		writergb_10(cur_rgb + rgb_line + bpp, bpp, cur_bay[1], cur_bay[bay_line + 1], cur_bay[bay_line + 2]);
		cur_bay += 2;
		cur_rgb += 2 * bpp;
	} while (--columns);
	/* Last lower-right pixel is handled here as special case */
	writergb_10(cur_rgb + 0, bpp, cur_bay[1], cur_bay[0], cur_bay[bay_line]);
	writergb_10(cur_rgb + bpp, bpp, cur_bay[1], cur_bay[bay_line + 1], cur_bay[bay_line]);
	writergb_10(cur_rgb + rgb_line, bpp, cur_bay[1], cur_bay[bay_line + 1], cur_bay[bay_line]);
	writergb_10(cur_rgb + rgb_line + bpp, bpp, cur_bay[1], cur_bay[bay_line + 1], cur_bay[bay_line]);
}

unsigned int Img_RAW::GetUnpackedBufferSize()
{
	if (m_pUnpackedData) {
		return m_uSize;
	}
	return 0;
}

int Img_RAW::SetLineBGR(unsigned char *pBGR, int nLine)
{
	unsigned char *pDst;
	int            nImageLineBytes;
	int            i;

	if (!m_pRGB) {
		return -1;
	}

	nImageLineBytes = m_nWidth * 3;
	pDst = m_pRGB + nImageLineBytes*nLine;
	for (i = 0; i < nImageLineBytes; i++) {
		pDst[i] = pBGR[i];
	}

	return 0;
}

int Img_RAW::RGBtoRAW(int nFormat)
{
	int i, j;
	
	if ( (nFormat == bayer_grbg_10bit_packed) || 
		 (nFormat == bayer_gbrg_10bit_packed)    ) 
	{
		if (!m_pData) {
			return -1;
		}
	}

	if ( (nFormat == bayer_grbg_10bit_unpacked) || 
		 (nFormat == bayer_gbrg_10bit_unpacked)    )
	{
		unsigned short *ptr;

		if (!m_pUnpackedData) {
			return -1;
		}
		ptr = (unsigned short *)m_pUnpackedData;


		for (i = 0; i < m_nHeight; i++) {
			for (j = 0; j < m_nWidth; j++) {
				ptr[i*m_nWidth + j]         = m_pRGB[i*m_nWidth*3 + j*3 + 1] << 2;   // G
				ptr[i*m_nWidth + j + 1]     = m_pRGB[i*m_nWidth*3 + j*3 + 2] << 2;   // R
				ptr[(i+1)*m_nWidth + j]     = m_pRGB[i*m_nWidth*3 + j*3] << 2;       // B
				ptr[(i+1)*m_nWidth + j + 1] = m_pRGB[i*m_nWidth*3 + j*3 + 1] << 2;   // G
				j++;
			}
			i++;
		}
	}

	return 0;
}
