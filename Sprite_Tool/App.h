#pragma once
#include "Window/Window.h"
#include "Manager/ManagerHeaders.h"
#include "UI\UIHeaders.h"
#include "Sprite/SpriteSheet.h"
#include "SpriteAnimation.h"

class App
{
public:
	App(_In_ HINSTANCE hInstance);
	~App();

	int Run();

private:
	void Update();
	void Render();

	void DrawSpriteSheet();
	void DrawBoundarySelectBox(const D2D1_RECT_F& rect);
	void DrawPivotPoint();
	void DrawMadeFrames();

	void SelectColorKey();
	void SelectArea();
	void SelectPivot();

	void MakeFrame();
	void MakePivot();


private:
	ImguiManager imGuiMan;
	Window wnd;

	D2D_POINT_2F sheetPos;
	D2D_POINT_2L pivotPos;

	D2D1_RECT_F boundaryBoxScreen;
	D2D1_RECT_L boundaryBoxPixel;

	SpriteSheet* pSpriteSheet;

private:
	ImUI_File fileUI;
	ImUI_ColorKeyAndMouse colorKeyAndMouseUI;
	ImUI_CurretAnimInfo currentAnimInfoUI;

private:
	std::vector<SpriteAnimation> anims;
	std::vector<Frame> framesInProcess;
};

