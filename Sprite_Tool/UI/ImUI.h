#pragma once

#include "../Window/Window.h"
#include "../imgui/imgui.h"
#include "../StringEncode.h"
#include <strsafe.h>
#include <atlstr.h>
#include <string>

using namespace std::string_literals;

class ImUI
{
public:
	ImUI(Window& wnd);
	virtual ~ImUI();

protected:
	Window& wnd;
	bool showUI;
};

