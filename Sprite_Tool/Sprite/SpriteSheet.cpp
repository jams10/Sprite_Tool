#include "SpriteSheet.h"
#include <assert.h>

SpriteSheet::SpriteSheet(IWICBitmap* pIBitmap, ID2D1Bitmap* pD2DBitmap)
	:
	pIBitmap(pIBitmap),
	pILock(nullptr),
	pD2DBitmap(pD2DBitmap),
	pData(nullptr),
	colorKey(0xffffffff)
{
}

SpriteSheet::~SpriteSheet()
{
	if (pILock)
	{
		pILock->Release();
		pILock = nullptr;
	}
	if (pIBitmap)
	{
		pIBitmap->Release();
		pIBitmap = nullptr;
	}
	if (pD2DBitmap)
	{
		pD2DBitmap->Release();
		pD2DBitmap = nullptr;
	}
	pData = nullptr;
}

void SpriteSheet::Lock()
{
	HRESULT hr;
	UINT width, height;

	// 비트맵 크기 얻어오기.
	pIBitmap->GetSize(&width, &height);
	// lock을 해줄 비트맵 영역.
	WICRect rcLock = { 0,0, width, height };

	// 비트맵 lock 획득.
	hr = pIBitmap->Lock(&rcLock, WICBitmapLockRead, &pILock);
	assert(hr == S_OK);

	UINT cbBufferSize = 0;

	// 실제 픽셀 데이터를 가리키는 포인터를 받아옴.
	hr = pILock->GetDataPointer(&cbBufferSize, &pData);
	assert(hr == S_OK);
}

void SpriteSheet::ReleaseLock()
{
	pILock->Release();
	pILock = nullptr;
	pData = nullptr;
}

DWORD SpriteSheet::GetPixelColor(UINT x, UINT y)
{
	Lock();

	DWORD color = 0x00000000;
	if (pILock == nullptr) return color;

	WICPixelFormatGUID pixelFormat;
	pILock->GetPixelFormat(&pixelFormat);
	//if (pixelFormat != GUID_WICPixelFormat32bppPBGRA)
	//{
	//	assert(pixelFormat == GUID_WICPixelFormat32bppPBGRA);
	//	return color; // 픽셀 포맷 체크. only png
	//}

	HRESULT hr = S_OK;
	UINT stride;

	hr = pILock->GetStride(&stride);
	assert(hr == S_OK);

	WICPixelFormatGUID format;
	pILock->GetPixelFormat(&format);

	if (format == GUID_WICPixelFormat32bppBGRA)
	{
		int offset = (y * stride) + (x * 4);
		BYTE blue = pData[offset];		// B
		BYTE green = pData[offset + 1]; // G
		BYTE red = pData[offset + 2];	// R
		BYTE alpha = pData[offset + 3]; // A
		color = color | (alpha << 24) | (red << 16) | (green << 8) | (blue);
	}


	ReleaseLock();

	return color;
}

D2D1_RECT_L SpriteSheet::CheckPixelBoundArea(D2D1_RECT_L selectedArea)
{
	Lock();

	D2D1_RECT_L minArea = { -1,-1,-1,-1 };
	if (pILock == nullptr) return minArea;

	WICPixelFormatGUID pixelFormat;
	pILock->GetPixelFormat(&pixelFormat);
	if (pixelFormat != GUID_WICPixelFormat32bppBGRA) return minArea; // 픽셀 포맷 체크. only png

	if (selectedArea.left < 0) selectedArea.left = 0;
	if (selectedArea.right >= pD2DBitmap->GetSize().width) selectedArea.right = pD2DBitmap->GetSize().width - 1;
	if (selectedArea.top < 0) selectedArea.top = 0;
	if (selectedArea.bottom >= pD2DBitmap->GetSize().height) selectedArea.bottom = pD2DBitmap->GetSize().height - 1;

	HRESULT hr = S_OK;
	UINT stride;

	hr = pILock->GetStride(&stride);
	assert(hr == S_OK);

	// left
	for (int x = selectedArea.left; x <= selectedArea.right; ++x)
	{
		for (int y = selectedArea.top; y <= selectedArea.bottom; ++y)
		{
			DWORD color = 0x00000000;
			int offset = (y * stride) + (x * 4);
			BYTE blue = pData[offset];		// B
			BYTE green = pData[offset + 1]; // G
			BYTE red = pData[offset + 2];	// R
			BYTE alpha = pData[offset + 3]; // A

			color = color | (alpha << 24) | (red << 16) | (green << 8) | (blue);
			if (color != colorKey)
			{
				minArea.left = x;
				break;
			}
		}
		if (minArea.left != -1) break;
	}
	if (minArea.left == -1)
	{
		ReleaseLock();
		return D2D1_RECT_L{ -1,-1,-1,-1 };
	}


	// top
	for (int y = selectedArea.top; y <= selectedArea.bottom; ++y)
	{
		for (int x = minArea.left; x <= selectedArea.right; ++x)
		{
			DWORD color = 0x00000000;
			int offset = (y * stride) + (x * 4);
			BYTE blue = pData[offset];		// B
			BYTE green = pData[offset + 1]; // G
			BYTE red = pData[offset + 2];	// R
			BYTE alpha = pData[offset + 3]; // A

			color = color | (alpha << 24) | (red << 16) | (green << 8) | (blue);
			if (color != colorKey)
			{
				minArea.top = y;
				break;
			}
		}
		if (minArea.top != -1) break;
	}
	if (minArea.top == -1)
	{
		ReleaseLock();
		return D2D1_RECT_L{ -1,-1,-1,-1 };
	}

	// right
	for (int x = selectedArea.right; x >= minArea.left; --x)
	{
		for (int y = minArea.top; y <= selectedArea.bottom; ++y)
		{
			DWORD color = 0x00000000;
			int offset = (y * stride) + (x * 4);
			BYTE blue = pData[offset];      // B
			BYTE green = pData[offset + 1]; // G
			BYTE red = pData[offset + 2];   // R
			BYTE alpha = pData[offset + 3]; // A

			color = color | (alpha << 24) | (red << 16) | (green << 8) | (blue);
			if (color != colorKey)
			{
				minArea.right = x;
				break;
			}
		}
		if (minArea.right != -1) break;
	}
	if (minArea.right == -1)
	{
		ReleaseLock();
		return D2D1_RECT_L{ -1,-1,-1,-1 };
	}

	// bottom
	for (int y = selectedArea.bottom; y >= minArea.top; --y)
	{
		for (int x = minArea.left; x <= minArea.right; ++x)
		{
			DWORD color = 0x00000000;
			int offset = (y * stride) + (x * 4);
			BYTE blue = pData[offset];      // B
			BYTE green = pData[offset + 1]; // G
			BYTE red = pData[offset + 2];   // R
			BYTE alpha = pData[offset + 3]; // A

			color = color | (alpha << 24) | (red << 16) | (green << 8) | (blue);
			if (color != colorKey)
			{
				minArea.bottom = y;
				break;
			}
		}
		if (minArea.bottom != -1) break;
	}
	if (minArea.bottom == -1)
	{
		ReleaseLock();
		return D2D1_RECT_L{ -1,-1,-1,-1 };
	}

	ReleaseLock();

	return minArea;
}
