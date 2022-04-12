#include "ImUI_File.h"
#include "../Sprite/SpriteSheet.h"

#pragma comment(lib, "Comdlg32.lib")

ImUI_File::ImUI_File(Window& wnd)
	:
	ImUI(wnd),
    fileName(L" ")
{
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = wnd.WND();

    ofn.lpstrFile = fileName;
    ofn.lpstrFilter = L"png(*.png)\0*.png";
    ofn.nMaxFile = MAX_PATH;
    ofn.nMaxFileTitle = MAX_PATH;
}

void ImUI_File::Run(SpriteSheet** pSpriteSheet)
{
    ImGui::Begin("Open/Save a file", &showUI);
    {
        if (ImGui::Button("OPEN"))
        {
            GetOpenFileName(&ofn);
            if (*pSpriteSheet != nullptr)
            {
                delete* pSpriteSheet;
                *pSpriteSheet = nullptr;
            }
            *pSpriteSheet = fileMan.GetSheetFromFile(wnd, fileName);
        }
        ImGui::SameLine();
        if (ImGui::Button("SAVE"))
        {
            GetSaveFileName(&ofn);
        }

        ImGui::Text(WideToMultiU8(L"! 현재 32bit 타입의 PNG 파일에만 동작합니다.").c_str());
        wchar_t tmp[128];
        StringCbPrintf(tmp, sizeof(tmp), L"파일 이름 : %s", fileName);
        ImGui::Text(WideToMultiU8(tmp).c_str());
    }
    ImGui::End();
}
