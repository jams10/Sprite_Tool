#pragma once
#include <vector>

struct Collision
{
	int left;
	int top;
	int right;
	int bottom;
};

struct Frame
{
	int originX;
	int originY;
	int left;
	int top;
	int right;
	int bottom;
	int nCollisions;
	std::vector<Collision> collisions;
};

struct SpriteAnimation
{
	wchar_t imageSourceName[128];
	wchar_t animationName[128];
	int nFrames;
	DWORD colorKey;
	std::vector<Frame> frames;
};

