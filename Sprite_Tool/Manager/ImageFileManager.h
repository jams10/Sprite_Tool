#pragma once
#include "../Window/WindowsHeaders.h"
#include "../Graphics/GraphicsHeaders.h"

class SpriteSheet;
class Window;
class ImageFileManager
{
public:
	ImageFileManager();
	~ImageFileManager();

	SpriteSheet* GetSheetFromFile(Window& wnd, wchar_t* fileName);
};

