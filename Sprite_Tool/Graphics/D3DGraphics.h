#pragma once
#include "GraphicsHeaders.h"

enum class BACKBUFFER
{
	INIT,
	RESIZE,
};

class D3DGraphics
{
public:
	D3DGraphics(HWND hWnd, UINT wndWidth, UINT wndHeight);
	~D3DGraphics();
	D3DGraphics(const D3DGraphics&) = delete;
	D3DGraphics& operator=(const D3DGraphics&) = delete;

	void CreateBackBuffer(UINT wndWidth, UINT wndHeight, BACKBUFFER mode);
	void DeleteBackBuffer();

	void BeginFrame();
	void EndFrame();

	IDXGISwapChain* GetSwapChain() const { return pSwap; }
	ID3D11Device* GetDevice() const { return pDevice; }
	UINT GetDPI() const { return wndDpi; }

private:
	IDXGISwapChain* pSwap;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	ID3D11RenderTargetView* pTarget;

	UINT wndDpi;
}; 

