#pragma once

#include "ImUI.h"
#include "../Manager/ImageFileManager.h"
#include <commdlg.h>

class ImUI_File : public ImUI
{
public:
	ImUI_File(Window& wnd);
	~ImUI_File() override {}

	void Run(SpriteSheet** pSpriteSheet);
	wchar_t* GetFileName() { return fileName; }

private:
	OPENFILENAME ofn;
	wchar_t fileName[256];
	ImageFileManager fileMan;
};