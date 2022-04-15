#pragma once
#include "ImUI.h"
#include "../Graphics/GraphicsHeaders.h"

class ImUI_DisplayAnim : public ImUI
{
public:
	ImUI_DisplayAnim(Window& wnd);
	~ImUI_DisplayAnim() override {}

	void Run(ID3D11ShaderResourceView* srv, int width, int height);

public:
	bool bPlayAnimation = false;
	float framespeed = 0.5f;
	float heightOffset = 0.5f;
};

