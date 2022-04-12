#pragma once

#include "../Window/WindowsHeaders.h"
#include "../Graphics/GraphicsHeaders.h"

class SpriteSheet
{
public:
	SpriteSheet(IWICBitmap* pIBitmap, ID2D1Bitmap* pD2DBitmap);
	~SpriteSheet();

	ID2D1Bitmap* GetD2DBitmap() { return pD2DBitmap; }
	D2D1_SIZE_F GetBitmapSize() { return pD2DBitmap->GetSize(); }

	void Lock();
	void ReleaseLock();

	void SetPixelColorKey(DWORD color) { colorKey = color; }
	DWORD GetColorKey() { return colorKey; }
	DWORD GetPixelColor(UINT x, UINT y);
	D2D1_RECT_L CheckPixelBoundArea(D2D1_RECT_L selectedArea);

private:
	IWICBitmap* pIBitmap;		    // 스프라이트 이미지의 픽셀 값 체크에 사용할 WIC 비트맵.
	IWICBitmapLock* pILock;         // 비트맵의 픽셀 데이터에 접근하기 위해 사용할 Lock
	ID2D1Bitmap* pD2DBitmap;		// 이미지 파일을 로드해 Direct2D 포맷으로 변환한 비트맵.
	BYTE* pData;					// 실제 픽셀 값 데이터를 가리키는 포인터.
	DWORD colorKey;                 // 스프라이트 애니메이션 출력 시, 경계 체크시에 사용할 컬러 키 값.
};

