#include "D2DGraphics.h"
#include "D3DGraphics.h"
#include <assert.h>

D2DGraphics::D2DGraphics(const D3DGraphics& D3DGraphics)
	:
	M_Origin(D2D1::Matrix3x2F::Identity()),
	M_Mouse(D2D1::Matrix3x2F::Identity()),
	M_Scale(D2D1::Matrix3x2F::Identity())
{
	HRESULT hr;

	hr = DWriteCreateFactory
	(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		(IUnknown**)&pWriteFactory
	);
	assert(hr == S_OK);

	D2D1_FACTORY_OPTIONS option;
	option.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr = D2D1CreateFactory
	(
		D2D1_FACTORY_TYPE_MULTI_THREADED,
		&pFactory
	);
	assert(hr == S_OK);

	IDXGIDevice* pDxgiDevice;
	hr = D3DGraphics.GetDevice()->QueryInterface(&pDxgiDevice);
	assert(hr == S_OK);

	hr = pFactory->CreateDevice(pDxgiDevice, &pDevice);
	assert(hr == S_OK);

	hr = pDevice->CreateDeviceContext
	(
		D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
		&pContext
	);
	assert(hr == S_OK);

	CreateBackBuffer(D3DGraphics);
}

D2DGraphics::~D2DGraphics()
{
	if (pFormat) pFormat->Release();
	if (pBrush) pBrush->Release();
	if (pBitmap) pBitmap->Release();
	if (pSurface) pSurface->Release();
	if (pContext) pContext->Release();
	if (pDevice) pDevice->Release();
	if (pWriteFactory) pWriteFactory->Release();
}

void D2DGraphics::CreateBackBuffer(const D3DGraphics& D3DGraphics)
{
	HRESULT hr = S_OK;

	if (D3DGraphics.GetSwapChain() == nullptr) return;

	// 스왑 체인에서 DXGI Surface를 얻어옴.
	hr = D3DGraphics.GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (void**)&pSurface);
	assert(hr == S_OK);
	
	// D2D 비트맵 속성 구조체 생성.
	D2D1_BITMAP_PROPERTIES1 bp;
	UINT dpi = D3DGraphics.GetDPI();
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bp.dpiX = dpi;
	bp.dpiY = dpi;
	bp.colorContext = NULL;

	// DXGI Surface로 부터 D2D 비트맵 생성.
	hr = pContext->CreateBitmapFromDxgiSurface(pSurface, &bp, &pBitmap);
	assert(hr == S_OK);

	// D2D의 렌더 타겟을 DXGI Surface로 부터 생성한 D2D 비트맵으로 설정.
	pContext->SetTarget(pBitmap);

	// 브러쉬 생성.
	hr = pContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &pBrush);
	assert(hr == S_OK);

	// 텍스트 포맷 생성.
	pWriteFactory->CreateTextFormat
	(
		L"돋움체", NULL,
		DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15, L"ko", &pFormat
	);

	// 스크린 중점이 (0,0)이 되도록 하는 행렬 값 업데이트.
	D2D1_SIZE_F size = pBitmap->GetSize();
	UpdateOriginMatrix(size.width, size.height);
}

void D2DGraphics::DeleteBackBuffer()
{
	if (pContext) pContext->SetTarget(NULL);
	else return;

	if (pBrush) pBrush->Release();

	if (pFormat) pFormat->Release();

	if (pBitmap) pBitmap->Release();

	if (pSurface) pSurface->Release();
}

void D2DGraphics::UpdateOriginMatrix(UINT width, UINT height)
{
	// 스크린 좌표 좌상단 (0,0)을 스크린의 중심으로 변환함.
	M_Origin = D2D1::Matrix3x2F::Translation(D2D1_SIZE_F{ width / 2.f, height / 2.f });
}

void D2DGraphics::UpdateMouseMatrix(float x, float y)
{
	M_Mouse = D2D1::Matrix3x2F::Translation(D2D1_SIZE_F{ x, y });
}

void D2DGraphics::UpdateScaleMatrix(float scale)
{
	D2D1_SIZE_F size = pBitmap->GetSize();
	// 스크린 중심 기준으로 scale 값 만큼 scale 변환.
	M_Scale = D2D1::Matrix3x2F::Scale(scale,scale, D2D1_POINT_2F{ size.width / 2.f, size.height / 2.f});
}

D2D1::Matrix3x2F D2DGraphics::GetInverseMatrix()
{
	D2D1::Matrix3x2F inversed = D2D1::Matrix3x2F::Identity();
	inversed = inversed * M_Origin * M_Mouse * M_Scale;
	D2D1InvertMatrix(&inversed);

	return inversed;
}

void D2DGraphics::DrawBitmap(ID2D1Bitmap& Bitmap)
{
	D2D1_SIZE_F bsize = Bitmap.GetSize();

	pContext->SetTransform(M_Origin * M_Mouse * M_Scale);

	pContext->DrawBitmap(
		&Bitmap,
		D2D1::RectF(-(bsize.width - bsize.width / 2), -(bsize.height - bsize.height / 2),
			(bsize.width - bsize.width / 2), (bsize.height - bsize.height / 2)),
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		D2D1::RectF(0.0f, 0.0f, bsize.width, bsize.height)
	);
}

void D2DGraphics::DrawBox(const D2D1_RECT_F& rect)
{
	pContext->DrawRectangle(rect, pBrush);
}

void D2DGraphics::DrawLine(D2D1_POINT_2F start, D2D1_POINT_2F end)
{
	pContext->DrawLine(start, end, pBrush);
}

