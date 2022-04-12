#pragma once
#include <vector>

struct Frame
{
	int originX;
	int originY;
	int left;
	int top;
	int right;
	int bottom;
};

struct SpriteAnimation
{
	wchar_t imageSourceName[128];
	int nFrames;
	std::vector<Frame> frames;
};

