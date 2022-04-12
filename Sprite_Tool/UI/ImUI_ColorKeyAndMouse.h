#pragma once

#include "ImUI.h"

class ImUI_ColorKeyAndMouse : public ImUI
{
public:
	ImUI_ColorKeyAndMouse(Window& wnd);
	~ImUI_ColorKeyAndMouse() override {}

	void Run();
	void UpdateColorKey(const DWORD& color) { colorKey = color; }
	void UpdateSelectBox(const D2D1_RECT_L& rect) { selectBox = rect; }
	void UpdatePivotPoint(const D2D1_POINT_2L& point) { pivotPoint = point; }

private:
	DWORD colorKey;
	D2D1_RECT_L selectBox;
	D2D1_POINT_2L pivotPoint;
};

