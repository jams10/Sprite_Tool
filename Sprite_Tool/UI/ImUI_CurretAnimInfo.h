#pragma once
#include "ImUI.h"
#include "../SpriteAnimation.h"

class ImUI_CurretAnimInfo : public ImUI
{
public:
	ImUI_CurretAnimInfo(Window& wnd);
	~ImUI_CurretAnimInfo() override {}

	void Run(std::vector<SpriteAnimation>& anims, wchar_t* fileName);

public:
	int currentFrameIndex;
	int currentAnimIndex;
	char buf[128] = "";
	bool bAddAnimation = false;
	bool is_selected = false;
};

