#pragma once

#include "ImUI.h"
#include "../Manager/ImageFileManager.h"
#include <commdlg.h>

struct SpriteAnimation;
class ImUI_File : public ImUI
{
public:
	ImUI_File(Window& wnd);
	~ImUI_File() override {}

	void Run(SpriteSheet** pSpriteSheet, const SpriteAnimation& animation);
	wchar_t* GetFileName() { return openFileName; }

private:
	OPENFILENAME openOFN;
	OPENFILENAME saveOFN;
	wchar_t openFileName[128];
	wchar_t saveFileName[128];
	ImageFileManager fileMan;
};