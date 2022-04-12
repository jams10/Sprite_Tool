#pragma once
#include "GraphicsHeaders.h"

class D3DGraphics;
class D2DGraphics
{
public:
	D2DGraphics(const D3DGraphics& D3DGraphics);
	~D2DGraphics();
	D2DGraphics(const D2DGraphics&) = delete;
	D2DGraphics& operator=(const D2DGraphics&) = delete;

	void CreateBackBuffer(const D3DGraphics& D3DGraphics);
	void DeleteBackBuffer();

	void BeginFrame() { pContext->BeginDraw(); }
	void EndFrame() { pContext->EndDraw(); }

	ID2D1DeviceContext* GetContext() { return pContext; }

public:
	void UpdateOriginMatrix(UINT width, UINT height);
	void UpdateMouseMatrix(float x, float y);
	void UpdateScaleMatrix(float scale);
	D2D1::Matrix3x2F GetInverseMatrix();


	void DrawBitmap(ID2D1Bitmap& Bitmap);
	void DrawBox(const D2D1_RECT_F& rect);
	void DrawLine(D2D1_POINT_2F start, D2D1_POINT_2F end);

	void SetBrushColor(const D2D1_COLOR_F& color) { pBrush->SetColor(color); }

private:
	ID2D1Factory1* pFactory;
	IDWriteFactory* pWriteFactory;

	ID2D1Device* pDevice;
	ID2D1DeviceContext* pContext;

	ID2D1Bitmap1* pBitmap;
	IDXGISurface* pSurface;

	ID2D1SolidColorBrush* pBrush;
	IDWriteTextFormat* pFormat;

	D2D1::Matrix3x2F M_Origin;
	D2D1::Matrix3x2F M_Mouse;
	D2D1::Matrix3x2F M_Scale;
};

