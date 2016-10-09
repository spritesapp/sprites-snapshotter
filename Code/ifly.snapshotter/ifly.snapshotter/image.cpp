#include "image.h"

#include <fstream>

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

void IflyImage::Write(int width, int height, const void* buffer, std::string outputFile) {
	ULONG_PTR mGdiplusToken;
	GdiplusStartupInput mGdiplusStartupInput;

	wchar_t* fn = new wchar_t[outputFile.length() + 1];
	std::copy(outputFile.begin(), outputFile.end(), fn);
	fn[outputFile.length()] = 0;

	GdiplusStartup(&mGdiplusToken, &mGdiplusStartupInput, NULL);
	{
		// Populate the bitmap info header.
		BITMAPINFOHEADER info;
		info.biSize = sizeof(BITMAPINFOHEADER);
		info.biWidth = width;
		info.biHeight = -height;  // minus means top-down bitmap
		info.biPlanes = 1;
		info.biBitCount = 32;
		info.biCompression = BI_RGB;  // no compression
		info.biSizeImage = 0;
		info.biXPelsPerMeter = 1;
		info.biYPelsPerMeter = 1;
		info.biClrUsed = 0;
		info.biClrImportant = 0;

		// Create the bitmap and retrieve the bit buffer.
		HDC screen_dc = GetDC(NULL);
		void* bits = NULL;
		HBITMAP bitmap = CreateDIBSection(screen_dc, reinterpret_cast<BITMAPINFO*>(&info), DIB_RGB_COLORS, &bits, NULL, NULL);
		ReleaseDC(NULL, screen_dc);

		// Read the image into the bit buffer.
		if (bitmap == NULL)
			return;

		memcpy(bits, buffer, width * height * 4);

		BITMAPINFO bi;
		bi.bmiHeader = info;

		SetDIBits(NULL, bitmap, 0, height, &bits, &bi, DIB_RGB_COLORS);

		Bitmap* bm = Bitmap::FromHBITMAP(bitmap, NULL);
		
		CLSID clsid;
		GetEncoderClsid(L"image/jpeg", &clsid);

		EncoderParameters encoderParameters;
		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;

		ULONG quality = 100;
		encoderParameters.Parameter[0].Value = &quality;

		bm->Save(fn, &clsid, &encoderParameters);
	}

	GdiplusShutdown(mGdiplusToken);
}
