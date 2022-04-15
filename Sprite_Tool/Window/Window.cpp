#include "Window.h"
#include "../imgui/imgui_impl_win32.h"

Window::Window(HINSTANCE hInst, int width, int height)
	:
	hInst(hInst)
{
	/* ������ Ŭ���� ���� �� ��� */
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = wndClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	/* ������ ���� */
	RECT rect = { 0, 0, width, height };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, wndClassName, L"Sprite Sheet Tool", WS_OVERLAPPEDWINDOW, 100, 100,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInst, this);

	// ������ ȭ�鿡 ����.
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	// Win32 imgui �ʱ�ȭ.
	ImGui_ImplWin32_Init(hWnd);

	// D3D ����� ���� �ν��Ͻ� ����.
	RECT clRect;
	GetClientRect(hWnd, &clRect);
	pD3DGraphics = std::make_unique<D3DGraphics>(hWnd, clRect.right - clRect.left, clRect.bottom - clRect.top);

	// D2D ����� ���� �ν��Ͻ� ����.
	pD2DGraphics = std::make_unique<D2DGraphics>(*pD3DGraphics);
}

Window::~Window()
{
	UnregisterClass(wndClassName,hInst);
}

bool Window::ProcessMessage()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			return false;
		}
	}
	return true;
}

std::pair<float, float> Window::GetScreenCoord(float x, float y)
{
	D2D1_POINT_2F point;
	point = pD2DGraphics->GetInverseMatrix().TransformPoint(D2D1_POINT_2F{ x,y });

	return std::pair<float, float>((int)round(point.x) + 0.5f, (int)round(point.y) + 0.5f);
}

std::pair<int, int> Window::GetSpriteCoord(float x, float y, const D2D1_SIZE_F& bitmapSize)
{
	D2D1_POINT_2F point;
	point = pD2DGraphics->GetInverseMatrix().TransformPoint(D2D1_POINT_2F{ x,y });

	return std::pair<int, int>(static_cast<int>(point.x + bitmapSize.width / 2.f), static_cast<int>(point.y + bitmapSize.height / 2.f));
}

LRESULT __stdcall Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// CreateWindow()�� ������ ���ڷ� �Ѱ��� this �����͸� �̿��� ������ API �ʿ��ִ� ������ Ŭ���� �����͸� ������.
	if (msg == WM_NCCREATE)
	{
		// ���� �����ͷ� ���� ������ Ŭ������ ���� �����͸� ����.
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// WinAPI�� ���� �����Ǵ� ����� �����͸� ������ Ŭ������ ���� �����͸� �����ϵ��� ������.
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		// �޽��� ���ν����� �Ϲ� �ڵ鷯�� �����ϰ� �޽����� �޾Ƶ��� �غ� ��.
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		// window class �ڵ鷯�� �޽����� ������.
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// WN_NCCREATE �޽��� ���� �޽����� �޴´ٸ�, �Ϲ� �ڵ鷯�� �޽����� ó��.
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT __stdcall Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// window class�� ���� �����͸� ����.
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	// window class �ڵ鷯�� �޽����� ������.
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Imgui�� �޽����� ó���� �� ������, �޽����� �Һ��ϵ��� ��.
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_SIZE:
	{
		if (!pD3DGraphics || !pD2DGraphics) break; // ���� �׷��Ƚ� ��ü���� �������� ���� ���� ���� ��ŵ.

		pD2DGraphics->DeleteBackBuffer();
		pD3DGraphics->DeleteBackBuffer();

		RECT rect;
		GetClientRect(hWnd, &rect);

		pD3DGraphics->CreateBackBuffer(rect.right - rect.left, rect.bottom - rect.top, BACKBUFFER::RESIZE);
		pD2DGraphics->CreateBackBuffer(*pD3DGraphics);

		pD2DGraphics->UpdateOriginMatrix(rect.right - rect.left, rect.bottom - rect.top);
	}
	break;
#pragma region Mouse
	case WM_RBUTTONDOWN:
	{
		moveMouse.isClicked = true;
		moveMouse.SetLastCoord(LOWORD(lParam), HIWORD(lParam));
	}
	break;
	case WM_RBUTTONUP:
	{
		moveMouse.isClicked = false;
		moveMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (bColorKeySelectMode) colorKeySelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bBoundarySelectMode) boundarySelectMouse.SetLastCoord(LOWORD(lParam), HIWORD(lParam));
		if (bPivotSelectMode) pivotSelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bCollisionSelectMode)collisionSelectMouse.SetLastCoord(LOWORD(lParam), HIWORD(lParam));

		colorKeySelectMouse.isClicked = true;
		boundarySelectMouse.isClicked = true;
		pivotSelectMouse.isClicked = true;
		collisionSelectMouse.isClicked = true;
	}
	break;
	case WM_LBUTTONUP:
	{
		if (bColorKeySelectMode) colorKeySelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bBoundarySelectMode) boundarySelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bPivotSelectMode) pivotSelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bCollisionSelectMode) collisionSelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));

		if (bBoundarySelectMode && boundarySelectMouse.isClicked) bAddFrame = true;
		if (bPivotSelectMode && pivotSelectMouse.isClicked) bAddPivot = true;
		if (bCollisionSelectMode && collisionSelectMouse.isClicked) bAddCollision = true;

		colorKeySelectMouse.isClicked = false;
		boundarySelectMouse.isClicked = false;
		pivotSelectMouse.isClicked = false;
		collisionSelectMouse.isClicked = false;
	}
	break;
	case WM_MOUSEMOVE:
	{
		moveMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));

		if (bColorKeySelectMode) colorKeySelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bBoundarySelectMode) boundarySelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bPivotSelectMode) pivotSelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
		if (bCollisionSelectMode) collisionSelectMouse.SetCurrentCoord(LOWORD(lParam), HIWORD(lParam));
	}
	break;
	case WM_MOUSEWHEEL:
	{
		if ((short)HIWORD(wParam) > 0) scale *= 2.f;
		else scale *= 0.5f;
	}
	break;
#pragma endregion
#pragma region Keyboard
	case WM_KEYDOWN:
	{	
		// QŰ
		if (wParam == 0x51 && !bBoundarySelectMode && !bPivotSelectMode && !bCollisionSelectMode) bColorKeySelectMode = true;
		// WŰ
		if (wParam == 0x57 && !bColorKeySelectMode && !bPivotSelectMode && !bCollisionSelectMode) bBoundarySelectMode = true;
		// EŰ
		if (wParam == 0x45 && !bColorKeySelectMode && !bBoundarySelectMode && !bCollisionSelectMode) bPivotSelectMode = true;
		// RŰ
		if (wParam == 0x52 && !bColorKeySelectMode && !bBoundarySelectMode && !bPivotSelectMode) bCollisionSelectMode = true;
	}
	break;
	case WM_KEYUP:
	{
		if (wParam == 0x51) bColorKeySelectMode = false;
		if (wParam == 0x57) bBoundarySelectMode = false;
		if (wParam == 0x45) bPivotSelectMode = false;
		if (wParam == 0x52) bCollisionSelectMode = false;
	}
	break;
#pragma endregion
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
