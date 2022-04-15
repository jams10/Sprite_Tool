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
        // PNG 파일 열기
        if (ImGui::Button(WideToMultiU8(L"이미지 파일 열기").c_str()))
        {
            GetOpenFileName(&openOFN);
            if (*pSpriteSheet != nullptr)
            {
                delete* pSpriteSheet;
                *pSpriteSheet = nullptr;
            }
            if (lstrcmpW(openFileName, L"") != 0)
            {
                // 파일로 부터 SpriteSheet 클래스 인스턴스 생성.
                *pSpriteSheet = fileMan.GetSheetFromFile(wnd, openFileName);
                // 만들어진 SpriteSheet 인스턴스를 이용, 애니메이션 표현을 위한 2D 텍스쳐 자원을 만들어줌.
                wnd.D3D().MakeTextureForAnimation(
                    200,
                    200,
                    (*pSpriteSheet)->GetStride());
            }
        }
        ImGui::SameLine();
        // 현재 작업중인 애니메이션 프레임을 다른 프로젝트에 사용할 수 있게 정보를 저장.
        if (ImGui::Button(WideToMultiU8(L"현재 선택한 애니메이션 저장").c_str()))
        {
            GetSaveFileName(&saveOFN);
            if (animation.nFrames > 0)
            {
                fileMan.SaveAnimationFile(animation, saveFileName);
            }
        }

        ImGui::Text(WideToMultiU8(L"! 현재 32bit 타입의 PNG 파일에만 동작합니다.").c_str());
        wchar_t tmp[128];
        StringCbPrintf(tmp, sizeof(tmp), L"스프라이트 시트 파일 이름 : %s", openFileName);
        ImGui::Text(WideToMultiU8(tmp).c_str());
    }
    ImGui::End();
}
