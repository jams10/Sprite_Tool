#include "D3DGraphics.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../Sprite/SpriteSheet.h"
#include "../SpriteAnimation.h"
#include <assert.h>

D3DGraphics::D3DGraphics(HWND hWnd, UINT wndWidth, UINT wndHeight)
{
	/* ���� ü��, ����̽�, ���ؽ�Ʈ ����. */
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = wndWidth;					  // backbuffer �ʺ�
	swapChainDesc.BufferDesc.Height = wndHeight;				  // backbuffer ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // �ȼ� ����
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;           // hz�� refresh rate ����
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;         // hz�� refresh rate �и�
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                 // �̹����� �־��� ������� �ػ󵵿� ���߱� ���� ��� �ÿ��������� ���� ����� ����Ŵ.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // surface�� �̹����� �����ϱ� ���� raster�� ����ϴ� ����� ����Ŵ.
	swapChainDesc.SampleDesc.Count = 1;                           // ���� ǥ��ȭ ����
	swapChainDesc.SampleDesc.Quality = 0;                         // ���� ǥ��ȭ ǰ��
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // ���۸� ����Ÿ�� �뵵�� ���.
	swapChainDesc.BufferCount = 1;                                // 1���� back buffer�� ���. �׷��� front/back ���� �� 2���� ���۸� ���.
	swapChainDesc.OutputWindow = hWnd;                            // ��� �����츦 ����.
	swapChainDesc.Windowed = TRUE;                                // â��带 ����.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;          // IDXGISwapChain1::Present1 ȣ�� �Ŀ� ���÷��� ȭ�鿡 �ִ� �ȼ����� ��� ó���� ���ΰ�? ���⼭�� �׳� ����.
	swapChainDesc.Flags = 0;                                      // �߰����� �÷��׵�.

	HRESULT hr = S_OK;

	// Direct2D���� ȣȯ���� ���� D3D11_CREATE_DEVICE_BGRA_SUPPORT �÷��� �߰�.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,                  // IDXGIAdapter�� nullptr�� ������, �⺻ ����͸� ��������.
		D3D_DRIVER_TYPE_HARDWARE, // ������ ����̹� Ÿ��.
		nullptr,                  // ����Ʈ���� �����Ͷ������� �����ϴ� DLL�� ���� �ڵ�.
		creationFlags,
		nullptr,                  // D3D_FEATURE_LEVEL. D3D ��ġ ���� ����. nullptr�� �ָ�, �⺻ 6���� ���� ������ ���.
		0,                        // pFeatureLevels�� �ִ� ���ҵ��� ����.
		D3D11_SDK_VERSION,        // SDK_VERSION. ���� �������� �ٸ�.
		&swapChainDesc,           // ���� ü�� ������
		&pSwap,                   // ������ ���� ü��
		&pDevice,                 // ������ ����̽�
		nullptr,                  // *pFeatureLevel. ������ ���ķ����� �־���. � ���� ������ �����Ǵ��� �� �ʿ� ������ �׳� nullptr.
		&pContext                 // ������ ����̽� ���ؽ�Ʈ
	);
	assert(hr == S_OK);

	// Back buffer ����.
	CreateBackBuffer(wndWidth, wndHeight, BACKBUFFER::INIT);

	// DPI ������.
	wndDpi = GetDpiForWindow(hWnd);

	// DX11 ������ imgui �ʱ�ȭ
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

// �� ���� ���� �Լ�. ������ �Ű������� ���� ũ�� �缳�� ���θ� ����.
void D3DGraphics::CreateBackBuffer(UINT wndWidth, UINT wndHeight, BACKBUFFER mode)
{
	HRESULT hr = S_OK;

	// D3DGraphics Ŭ���� �ν��Ͻ� ���� �����̸�, �׳� ����.
	if (pSwap == nullptr) return;

	// ������¡�� ��� ���� ������ ũ�⸦ �� ũ��� ��������.
	if (mode == BACKBUFFER::RESIZE)
	{
		hr = pSwap->ResizeBuffers(0, wndWidth, wndHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		assert(hr == S_OK);
	}

	/* ���� Ÿ�� �� ����. */
	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (pBackBuffer != nullptr)
	{
		hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
		assert(hr == S_OK);

		pBackBuffer->Release();
	}

	/* �� ��Ʈ ���� �� ���ε�. */
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
	pContext->OMSetRenderTargets(1u, &pTarget, nullptr); //	���� Ÿ�� �� ���ε�.

	ImGui_ImplDX11_NewFrame(); // �� �������� �׷��ֱ� ���� ȣ���� �־�� ��.
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	const float color[] = { 163.f/255.f,204.f/255.f,163.f/255.f,0.5f };

	pContext->ClearRenderTargetView(pTarget, color);	 // ���� Ÿ�� �並 �ʱ�ȭ.
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
	desc.Usage = D3D11_USAGE_DYNAMIC;             // �ؽ��� �ڿ� ������Ʈ�� ���� ���� �ڿ� ����.
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // �ؽ��� �ڿ� ������Ʈ�� ���� CPU ���� ��� ����.

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

	// ��������Ʈ ��Ʈ �̹����� ��� �ִ� IWICBitmap�� �ȼ� �����Ϳ� �����ϱ� ���� �����͸� ������.
	DWORD *pSheet = (DWORD*)pSpriteSheet->GetPixelDataPointer();

	// GPU�� ���ε��� �ؽ��� �ڿ��� �����ϱ� ���� lock
	hr = pContext->Map(pTexture, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubResource);
	assert(hr == S_OK);

	UINT stride = pSpriteSheet->GetStride();
	DWORD colorKey = pSpriteSheet->GetColorKey();
	D2D1_SIZE_F spriteSheetSize = pSpriteSheet->GetBitmapSize();

	DWORD* pDst = reinterpret_cast<DWORD*>(mappedSubResource.pData);
	const size_t dstPitch = mappedSubResource.RowPitch / sizeof(DWORD);

	// �ϴ� �ִϸ��̼� �ؽ��� �̹��� ũ�⸸ŭ�� ������ ��������� �ʱ�ȭ.
	for (int y = 0; y < animTextureHeight; ++y)
		for (int x = 0; x < animTextureWidth; ++x)
			pDst[y * dstPitch + x] = 0x0fffffff;

	// ������ ������ �ڽ� ũ�Ⱑ 0�̻� ���� üũ.
	int frameheight = frame.bottom - frame.top;
	int framewidth = frame.right - frame.left;

	if (frameheight <= 0 && framewidth <= 0)
	{
		pContext->Unmap(pTexture, 0u);
		pSpriteSheet->ReleaseLock();
		return;
	}

	// frame �����ȿ� ������ �ȼ��� GPU�� ����� �ؽ��� �ڿ��� ���ֱ�.
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
