#pragma once
#include "Window/Window.h"
#include "Manager/ManagerHeaders.h"
#include "UI\UIHeaders.h"
#include "Sprite/SpriteSheet.h"
#include "SpriteAnimation.h"
#include "Timer.h"

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
	void DrawSelectBox(D2D1_RECT_F& selectBox, const D2D1_COLOR_F& color);
	void DrawPivotPoint();
	void DrawMadeFrames();

	void SelectColorKey();
	void SelectArea(const Mouse& mouse, D2D1_RECT_F& screen, D2D1_RECT_L& pixel);
	void SelectPivot();

	void MakeFrame();
	void MakePivot();
	void MakeCollision();


private:
	ImguiManager imGuiMan;
	Window wnd;

	D2D_POINT_2F sheetPos;
	D2D_POINT_2L pivotPos;

	D2D1_RECT_F boundaryBoxScreen;
	D2D1_RECT_L boundaryBoxPixel;

	D2D1_RECT_F collisionBoxScreen;
	D2D1_RECT_L collisionBoxPixel;

	SpriteSheet* pSpriteSheet;

private:
	ImUI_File fileUI;
	ImUI_ColorKeyAndMouse colorKeyAndMouseUI;
	ImUI_CurretAnimInfo currentAnimInfoUI;
	ImUI_DisplayAnim displayAnimUI;

private:
	std::vector<SpriteAnimation> anims;

	Timer timer;
	float elapsedTime;
	float holdTime;

	int currentSelectAnimIndex;
	int currentSelectFrameIndex;
	int currentAnimFrameIndex;
};

