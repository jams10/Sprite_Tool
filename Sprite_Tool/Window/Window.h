#pragma once
#include "WindowsHeaders.h"
#include "../Graphics/D3DGraphics.h"
#include "../Graphics/D2DGraphics.h"
#include "../Mouse.h"
#include <memory>

class Window
{
public:
	Window(HINSTANCE hInst, int width, int height);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();
	
	bool ProcessMessage();
	D3DGraphics& D3D() { return *pD3DGraphics; }
	D2DGraphics& D2D() { return *pD2DGraphics; }
	HWND& WND() { return hWnd; }

	std::pair<float, float>	GetScreenCoord(float x, float y);
	std::pair<int, int>	GetSpriteCoord(float x, float y, const D2D1_SIZE_F& bitmapSize);

private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	const wchar_t* wndClassName = L"Window Class";
	HINSTANCE hInst = nullptr;
	HWND hWnd;

	std::unique_ptr<D3DGraphics> pD3DGraphics;
	std::unique_ptr<D2DGraphics> pD2DGraphics;

public:
	// mouse
	Mouse moveMouse;
	Mouse colorKeySelectMouse;
	Mouse boundarySelectMouse;
	Mouse pivotSelectMouse;
	Mouse collisionSelectMouse;
	
	float scale = 1.f;

	// mode
	bool bColorKeySelectMode = false;
	bool bBoundarySelectMode = false;
	bool bAddFrame = false;
	bool bPivotSelectMode = false;
	bool bAddPivot = false;
	bool bCollisionSelectMode = false;
	bool bAddCollision = false;
};

