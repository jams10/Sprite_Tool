#pragma once
#include "GraphicsHeaders.h"

enum class BACKBUFFER
{
	INIT,
	RESIZE,
};

class SpriteSheet;
struct Frame;
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
	ID3D11ShaderResourceView* GetShaderResourceView() { return pSrv; }

	void MakeTextureForAnimation(UINT width, UINT height, UINT stride);
	void UpdateTextureForAnimation(const Frame& frame, SpriteSheet* pSpriteSheet, float heightOffset);
	int GetAnimTextureWidth() { return animTextureWidth; }
	int GetAnimTextureHeight() { return animTextureHeight; }

private:
	IDXGISwapChain* pSwap = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
	ID3D11Texture2D* pTexture = nullptr;
	ID3D11ShaderResourceView* pSrv = nullptr;
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;

	UINT wndDpi;
	UINT animTextureWidth;
	UINT animTextureHeight;
}; 

