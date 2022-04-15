#include "ImUI_File.h"
#include "../Sprite/SpriteSheet.h"
#include "../SpriteAnimation.h"

#pragma comment(lib, "Comdlg32.lib")

ImUI_File::ImUI_File(Window& wnd)
	:
	ImUI(wnd),
    openFileName(L" "),
    saveFileName(L" ")
{
    memset(&openOFN, 0, sizeof(OPENFILENAME));
    openOFN.lStructSize = sizeof(OPENFILENAME);
    openOFN.hwndOwner = wnd.WND();

    openOFN.lpstrFile = openFileName;
    openOFN.lpstrFilter = L"png(*.png)\0*.png";
    openOFN.nMaxFile = MAX_PATH;
    openOFN.nMaxFileTitle = MAX_PATH;

    memset(&saveOFN, 0, sizeof(OPENFILENAME));
    saveOFN.lStructSize = sizeof(OPENFILENAME);
    saveOFN.hwndOwner = wnd.WND();

    saveOFN.lpstrFile = saveFileName;
    saveOFN.lpstrFilter = L"spra(*.spra)\0*.spra";
    saveOFN.nMaxFile = MAX_PATH;
    saveOFN.nMaxFileTitle = MAX_PATH;
}

void ImUI_File::Run(SpriteSheet** pSpriteSheet, const SpriteAnimation& animation)
{
    ImGui::Begin("Open/Save a file", &showUI);
    {
        // PNG ���� ����
        if (ImGui::Button(WideToMultiU8(L"�̹��� ���� ����").c_str()))
        {
            GetOpenFileName(&openOFN);
            if (*pSpriteSheet != nullptr)
            {
                delete* pSpriteSheet;
                *pSpriteSheet = nullptr;
            }
            if (lstrcmpW(openFileName, L"") != 0)
            {
                // ���Ϸ� ���� SpriteSheet Ŭ���� �ν��Ͻ� ����.
                *pSpriteSheet = fileMan.GetSheetFromFile(wnd, openFileName);
                // ������� SpriteSheet �ν��Ͻ��� �̿�, �ִϸ��̼� ǥ���� ���� 2D �ؽ��� �ڿ��� �������.
                wnd.D3D().MakeTextureForAnimation(
                    200,
                    200,
                    (*pSpriteSheet)->GetStride());
            }
        }
        ImGui::SameLine();
        // ���� �۾����� �ִϸ��̼� �������� �ٸ� ������Ʈ�� ����� �� �ְ� ������ ����.
        if (ImGui::Button(WideToMultiU8(L"���� ������ �ִϸ��̼� ����").c_str()))
        {
            GetSaveFileName(&saveOFN);
            if (animation.nFrames > 0)
            {
                fileMan.SaveAnimationFile(animation, saveFileName);
            }
        }

        ImGui::Text(WideToMultiU8(L"! ���� 32bit Ÿ���� PNG ���Ͽ��� �����մϴ�.").c_str());
        wchar_t tmp[128];
        StringCbPrintf(tmp, sizeof(tmp), L"��������Ʈ ��Ʈ ���� �̸� : %s", openFileName);
        ImGui::Text(WideToMultiU8(tmp).c_str());
    }
    ImGui::End();
}
