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
	IWICBitmap* pIBitmap;		    // ��������Ʈ �̹����� �ȼ� �� üũ�� ����� WIC ��Ʈ��.
	IWICBitmapLock* pILock;         // ��Ʈ���� �ȼ� �����Ϳ� �����ϱ� ���� ����� Lock
	ID2D1Bitmap* pD2DBitmap;		// �̹��� ������ �ε��� Direct2D �������� ��ȯ�� ��Ʈ��.
	BYTE* pData;					// ���� �ȼ� �� �����͸� ����Ű�� ������.
	DWORD colorKey;                 // ��������Ʈ �ִϸ��̼� ��� ��, ��� üũ�ÿ� ����� �÷� Ű ��.
};

