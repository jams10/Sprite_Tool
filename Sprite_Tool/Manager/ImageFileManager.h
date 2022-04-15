#pragma once
#include "../Window/WindowsHeaders.h"
#include "../Graphics/GraphicsHeaders.h"

class SpriteSheet;
class Window;
struct SpriteAnimation;
class ImageFileManager
{
public:
	ImageFileManager();
	~ImageFileManager();

	SpriteSheet* GetSheetFromFile(Window& wnd, wchar_t* fileName);
	void SaveAnimationFile(const SpriteAnimation& animation, wchar_t* fileName);
};

