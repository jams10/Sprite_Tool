#pragma once
#include "ImUI.h"
#include "../SpriteAnimation.h"

class ImUI_CurretAnimInfo : public ImUI
{
public:
	ImUI_CurretAnimInfo(Window& wnd);
	~ImUI_CurretAnimInfo() override {}

	void Run(const std::vector<Frame> frames);

public:
	int currentFrameIndex;
};

