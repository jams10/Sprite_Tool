#pragma once
#include <cmath>

class Mouse
{
public:
	Mouse()
		:
		isClicked(false),
		lastRawX(0.f), lastRawY(0.f),
		curRawX(0.f), curRawY(0.f)

	{
	}
	~Mouse() = default;

	void SetLastCoord(float x, float y) { lastRawX = x; lastRawY = y; }
	void SetCurrentCoord(float x, float y) { curRawX = x; curRawY = y; }

	float GetLastRawX() const { return lastRawX; }
	float GetLastRawY() const { return lastRawY; }
	float GetCurRawX() const { return curRawX; }
	float GetCurRawY() const { return curRawY; }

	static float GetScreenFromPixel(int a, float size)
	{
		return round(a - size / 2.f) + 0.5f;
	}

public:
	bool isClicked;

private:
	float lastRawX;
	float lastRawY;
	float curRawX;
	float curRawY;
};

