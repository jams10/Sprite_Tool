#include "D3DGraphics.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../Sprite/SpriteSheet.h"
#include "../SpriteAnimation.h"
#include <assert.h>

D3DGraphics::D3DGraphics(HWND hWnd, UINT wndWidth, UINT wndHeight)
{
	/* 스왑 체인, 디바이스, 컨텍스트 생성. */
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = wndWidth;					  // backbuffer 너비
	swapChainDesc.BufferDesc.Height = wndHeight;				  // backbuffer 높이
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 픽셀 형식
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;           // hz의 refresh rate 분자
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;         // hz의 refresh rate 분모
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                 // 이미지가 주어진 모니터의 해상도에 맞추기 위해 어떻게 늘여지는지에 대한 방법을 가리킴.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // surface에 이미지를 생성하기 위해 raster가 사용하는 방법을 가리킴.
	swapChainDesc.SampleDesc.Count = 1;                           // 다중 표본화 개수
	swapChainDesc.SampleDesc.Quality = 0;                         // 다중 표본화 품질
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // 버퍼를 렌더타겟 용도로 사용.
	swapChainDesc.BufferCount = 1;                                // 1개의 back buffer를 사용. 그러면 front/back 으로 총 2개의 버퍼를 사용.
	swapChainDesc.OutputWindow = hWnd;                            // 출력 윈도우를 지정.
	swapChainDesc.Windowed = TRUE;                                // 창모드를 적용.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;          // IDXGISwapChain1::Present1 호출 후에 디스플레이 화면에 있는 픽셀들을 어떻게 처리할 것인가? 여기서는 그냥 버림.
	swapChainDesc.Flags = 0;                                      // 추가적인 플래그들.

	HRESULT hr = S_OK;

	// Direct2D와의 호환성을 위해 D3D11_CREATE_DEVICE_BGRA_SUPPORT 플래그 추가.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,                  // IDXGIAdapter를 nullptr로 설정해, 기본 어댑터를 선택해줌.
		D3D_DRIVER_TYPE_HARDWARE, // 생성할 드라이버 타입.
		nullptr,                  // 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들.
		creationFlags,
		nullptr,                  // D3D_FEATURE_LEVEL. D3D 장치 피쳐 레벨. nullptr로 주면, 기본 6가지 피쳐 레벨을 사용.
		0,                        // pFeatureLevels에 있는 원소들의 개수.
		D3D11_SDK_VERSION,        // SDK_VERSION. 피쳐 레벨과는 다름.
		&swapChainDesc,           // 스왑 체인 서술자
		&pSwap,                   // 생성된 스왑 체인
		&pDevice,                 // 생성된 디바이스
		nullptr,                  // *pFeatureLevel. 가능한 피쳐레벨을 넣어줌. 어떤 피쳐 레벨이 지원되는지 알 필요 없으면 그냥 nullptr.
		&pContext                 // 생성된 디바이스 컨텍스트
	);
	assert(hr == S_OK);

	// Back buffer 생성.
	CreateBackBuffer(wndWidth, wndHeight, BACKBUFFER::INIT);

	// DPI 얻어오기.
	wndDpi = GetDpiForWindow(hWnd);

	// DX11 렌더링 imgui 초기화
	ImGui_ImplDX11_Init(pDevice, pContext);
}

D3DGraphics::~D3DGraphics()
{
	if (pSrv) pTexture->Release();
	if (pTexture) pTexture->Release();
	if (pTarget) pTarget->Release();
	if (pSwap) pSwap->Release();
	if (pContext) pContext->Release();
	if (pDevice) pDevice->Release();
}

// 백 버퍼 생성 함수. 마지막 매개변수에 따라 크기 재설정 여부를 결정.
void D3DGraphics::CreateBackBuffer(UINT wndWidth, UINT wndHeight, BACKBUFFER mode)
{
	HRESULT hr = S_OK;

	// D3DGraphics 클래스 인스턴스 생성 이전이면, 그냥 리턴.
	if (pSwap == nullptr) return;

	// 리사이징인 경우 기존 버퍼의 크기를 새 크기로 설정해줌.
	if (mode == BACKBUFFER::RESIZE)
	{
		hr = pSwap->ResizeBuffers(0, wndWidth, wndHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		assert(hr == S_OK);
	}

	/* 렌더 타겟 뷰 생성. */
	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (pBackBuffer != nullptr)
	{
		hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
		assert(hr == S_OK);

		pBackBuffer->Release();
	}

	/* 뷰 포트 생성 및 바인딩. */
	{
		D3D11_VIEWPORT viewPort;
		ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		viewPort.Width = wndWidth;
		viewPort.Height = wndHeight;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 0.0f;

		pContext->RSSetViewports(1u, &viewPort);
	}
}

void D3DGraphics::DeleteBackBuffer()
{
	if (pTarget) pTarget->Release();
	pTarget = nullptr;
}

void D3DGraphics::BeginFrame()
{
	pContext->OMSetRenderTargets(1u, &pTarget, nullptr); //	렌더 타겟 뷰 바인딩.

	ImGui_ImplDX11_NewFrame(); // 새 프레임을 그려주기 전에 호출해 주어야 함.
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	const float color[] = { 163.f/255.f,204.f/255.f,163.f/255.f,0.5f };

	pContext->ClearRenderTargetView(pTarget, color);	 // 렌더 타겟 뷰를 초기화.
}

void D3DGraphics::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	pSwap->Present(0, 0);
}

void D3DGraphics::MakeTextureForAnimation(UINT width, UINT height, UINT stride)
{
	if (pTexture != nullptr) pTexture->Release();

	animTextureWidth = width;
	animTextureHeight = height;

	DWORD* pData = new DWORD[width * height];

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			pData[y * width + x] = 0xffffffff;
		}
	}

	HRESULT hr = S_OK;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;             // 텍스쳐 자원 업데이트를 위한 동적 자원 설정.
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 텍스쳐 자원 업데이트를 위한 CPU 접근 허용 설정.

	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = pData;
	subResource.SysMemPitch = width * sizeof(DWORD);
	subResource.SysMemSlicePitch = 0;
	hr = pDevice->CreateTexture2D(&desc, &subResource, &pTexture);
	assert(hr == S_OK);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	if (pTexture != nullptr)
	{
		hr = pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSrv);
		assert(hr == S_OK);
	}

	delete [] pData;
}

void D3DGraphics::UpdateTextureForAnimation(const Frame& frame, SpriteSheet* pSpriteSheet, float heightOffset)
{
	HRESULT hr = S_OK;

	pSpriteSheet->Lock();

	// 스프라이트 시트 이미지를 담고 있는 IWICBitmap의 픽셀 데이터에 접근하기 위한 포인터를 가져옴.
	DWORD *pSheet = (DWORD*)pSpriteSheet->GetPixelDataPointer();

	// GPU에 바인딩된 텍스쳐 자원에 접근하기 위해 lock
	hr = pContext->Map(pTexture, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubResource);
	assert(hr == S_OK);

	UINT stride = pSpriteSheet->GetStride();
	DWORD colorKey = pSpriteSheet->GetColorKey();
	D2D1_SIZE_F spriteSheetSize = pSpriteSheet->GetBitmapSize();

	DWORD* pDst = reinterpret_cast<DWORD*>(mappedSubResource.pData);
	const size_t dstPitch = mappedSubResource.RowPitch / sizeof(DWORD);

	// 일단 애니메이션 텍스쳐 이미지 크기만큼의 영역을 투명색으로 초기화.
	for (int y = 0; y < animTextureHeight; ++y)
		for (int x = 0; x < animTextureWidth; ++x)
			pDst[y * dstPitch + x] = 0x0fffffff;

	// 선택한 프레임 박스 크기가 0이상 인지 체크.
	int frameheight = frame.bottom - frame.top;
	int framewidth = frame.right - frame.left;

	if (frameheight <= 0 && framewidth <= 0)
	{
		pContext->Unmap(pTexture, 0u);
		pSpriteSheet->ReleaseLock();
		return;
	}

	// frame 영역안에 들어오는 픽셀을 GPU에 저장된 텍스쳐 자원에 써주기.
	int displayOffsetX = animTextureWidth / 2;
	int pivotOffsetX = frame.originX;
	displayOffsetX -= pivotOffsetX;

	int displayOffsetY = animTextureHeight * heightOffset - frame.originY;

	for (int y = 0; y < animTextureHeight - displayOffsetY; ++y)
	{
		for (int x = displayOffsetX; x < animTextureWidth; ++x)
		{
			if (y + frame.top <= frame.bottom && x + frame.left - displayOffsetX <= frame.right)
			{
				int sheetOffset = (y + frame.top) * static_cast<int>(spriteSheetSize.width) +
								   x + frame.left - displayOffsetX;
				if (pSheet[sheetOffset] != colorKey)
					pDst[(y + displayOffsetY)*dstPitch + x] = pSheet[sheetOffset];
			}
		}
	}

	pContext->Unmap(pTexture, 0u);

	pSpriteSheet->ReleaseLock();
}
