#pragma once

#include "../Window/Window.h"
#include "../imgui/imgui.h"
#include "../StringEncode.h"
#include <strsafe.h>

class ImUI
{
public:
	ImUI(Window& wnd);
	virtual ~ImUI();

protected:
	Window& wnd;
	bool showUI;
};

