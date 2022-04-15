#include "ImUI.h"
#include <clocale>

ImUI::ImUI(Window& wnd)
	:
	wnd(wnd)
{
	setlocale(LC_ALL, "");
}

ImUI::~ImUI()
{
}


