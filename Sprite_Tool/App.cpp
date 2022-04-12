#include "App.h"
#include "imgui\imgui.h"
#include <strsafe.h>

#define xCoord first
#define yCoord second

App::App(_In_ HINSTANCE hInstance)
    :
    wnd(Window(hInstance, 1080, 720)),
    pSpriteSheet(nullptr),
    sheetPos(D2D1_POINT_2F{0.f,0.f}),
    fileUI(wnd),
    colorKeyAndMouseUI(wnd),
    currentAnimInfoUI(wnd)
{
}

App::~App()
{
}

int App::Run()
{
    while (wnd.ProcessMessage())
    {
        Update();
        Render();
    }
    return 0;
}

void App::Update()
{
    // 스프라이트 시트 이동을 위한 마우스 좌표 업데이트.
    if (wnd.moveMouse.isClicked)
    {
        sheetPos.x += (wnd.moveMouse.GetCurRawX() - wnd.moveMouse.GetLastRawX());
        sheetPos.y += (wnd.moveMouse.GetCurRawY() - wnd.moveMouse.GetLastRawY());
        wnd.moveMouse.SetLastCoord(wnd.moveMouse.GetCurRawX(), wnd.moveMouse.GetCurRawY());
    }

    if (pSpriteSheet != nullptr)
    {
        // 1. Color Key 설정.
        if (wnd.bColorKeySelectMode && wnd.colorKeySelectMouse.isClicked)
            SelectColorKey();
        // 2. 프레임 경계 선택 및 추가.
        else if (wnd.bBoundarySelectMode && wnd.boundarySelectMouse.isClicked)
            SelectArea();
        if (wnd.bAddFrame)
        {
            MakeFrame();
            wnd.bAddFrame = false;
        }
        // 3. 프레임 피봇 기준점 선택 및 추가.
        if (wnd.bPivotSelectMode)
        {
            SelectPivot();
        }
        if (wnd.bAddPivot)
        {
            MakePivot();
            wnd.bAddPivot = false;
        }
    }

    // 스프라이트 시트 변환 행렬 업데이트.
    wnd.D2D().UpdateMouseMatrix(sheetPos.x, sheetPos.y);
    wnd.D2D().UpdateScaleMatrix(wnd.scale);

}

void App::Render()
{
    wnd.D3D().BeginFrame();
    wnd.D2D().BeginFrame();

    // 1. 스프라이트 시트 그리기.
    DrawSpriteSheet();
    // 2. 픽셀 경계 체크용 선택 박스 그리기.
    if(wnd.bBoundarySelectMode)
        DrawBoundarySelectBox(boundaryBoxScreen);
    // 3. 피봇 선택 지점 그리기.
    if (wnd.bPivotSelectMode && currentAnimInfoUI.currentFrameIndex >= 0)
    {
        DrawPivotPoint();
    }

    // 작업이 끝난 프레임 선택 지점들 그리기.
    DrawMadeFrames();

    // UI 그리기
    fileUI.Run(&pSpriteSheet);
    colorKeyAndMouseUI.Run();
    currentAnimInfoUI.Run(framesInProcess);

    wnd.D2D().EndFrame();
    wnd.D3D().EndFrame();
}

#pragma region DrawStuff
void App::DrawSpriteSheet()
{
    if(pSpriteSheet)
        wnd.D2D().DrawBitmap(*pSpriteSheet->GetD2DBitmap());
}

void App::DrawBoundarySelectBox(const D2D1_RECT_F& rect)
{
    wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 1.f,1.f,1.f,1.f });
    wnd.D2D().DrawBox(rect);

    // 이전에 선택한 부분을 다시 표시하지 않기 위해 초기화.
    boundaryBoxScreen = D2D1_RECT_F{ -1.f, -1.f, -1.f, -1.f };
}

void App::DrawPivotPoint()
{
    wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 0.f,0.f,1.f,1.f });

    float x = Mouse::GetScreenFromPixel(pivotPos.x, pSpriteSheet->GetBitmapSize().width);
    float y1 = Mouse::GetScreenFromPixel(framesInProcess[currentAnimInfoUI.currentFrameIndex].top, pSpriteSheet->GetBitmapSize().height);
    float y2 = Mouse::GetScreenFromPixel(framesInProcess[currentAnimInfoUI.currentFrameIndex].bottom, pSpriteSheet->GetBitmapSize().height);

    wnd.D2D().DrawLine(D2D1_POINT_2F{ x, y1 }, D2D1_POINT_2F{ x, y2 });
}

void App::DrawMadeFrames()
{
    for (const Frame& f : framesInProcess)
    {
        wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 1.f,1.f,1.f,0.2f });
        // 스크린 좌표로 변환 하여 프레임 선택 박스 그리기.
        wnd.D2D().DrawBox(D2D1_RECT_F(
            Mouse::GetScreenFromPixel(f.left, pSpriteSheet->GetBitmapSize().width),
            Mouse::GetScreenFromPixel(f.top, pSpriteSheet->GetBitmapSize().height),
            Mouse::GetScreenFromPixel(f.right, pSpriteSheet->GetBitmapSize().width),
            Mouse::GetScreenFromPixel(f.bottom, pSpriteSheet->GetBitmapSize().height)
        ));
        wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 0.f,0.f,1.f,0.5f });
        // 스크린 좌표로 변환하여 선택 pivot 선 그리기.
        wnd.D2D().DrawLine(
            D2D1_POINT_2F(
                Mouse::GetScreenFromPixel(f.originX, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.top, pSpriteSheet->GetBitmapSize().height)),
            D2D1_POINT_2F(
                Mouse::GetScreenFromPixel(f.originX, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.bottom, pSpriteSheet->GetBitmapSize().height))
        );
    }
}

#pragma endregion

#pragma region SelectStuff
void App::SelectColorKey()
{
    std::pair<int, int> colorKeyPos = wnd.GetSpriteCoord(
        wnd.colorKeySelectMouse.GetCurRawX(), 
        wnd.colorKeySelectMouse.GetCurRawY(),
        pSpriteSheet->GetBitmapSize());

    // 컬러 키 업데이트.
    DWORD color = pSpriteSheet->GetPixelColor(colorKeyPos.xCoord, colorKeyPos.yCoord);
    pSpriteSheet->SetPixelColorKey(color);
    // UI 표시 색상 업데이트.
    colorKeyAndMouseUI.UpdateColorKey(color);
}

void App::SelectArea()
{
    // 선택 영역 좌표 업데이트. (스크린 중앙이 원점인 좌표)
    std::pair<float, float> startS = wnd.GetScreenCoord(wnd.boundarySelectMouse.GetLastRawX(), wnd.boundarySelectMouse.GetLastRawY());
    std::pair<float, float> endS = wnd.GetScreenCoord(wnd.boundarySelectMouse.GetCurRawX(), wnd.boundarySelectMouse.GetCurRawY());
    boundaryBoxScreen.left = startS.xCoord;
    boundaryBoxScreen.top = startS.yCoord;
    boundaryBoxScreen.right = endS.xCoord;
    boundaryBoxScreen.bottom = endS.yCoord;

    // 선택 영역 픽셀 좌표.
    std::pair<int, int> startP = wnd.GetSpriteCoord(
        wnd.boundarySelectMouse.GetLastRawX(), 
        wnd.boundarySelectMouse.GetLastRawY(),
        pSpriteSheet->GetBitmapSize());
    std::pair<int, int> endP = wnd.GetSpriteCoord(
        wnd.boundarySelectMouse.GetCurRawX(), 
        wnd.boundarySelectMouse.GetCurRawY(),
        pSpriteSheet->GetBitmapSize());
    boundaryBoxPixel.left = startP.xCoord;
    boundaryBoxPixel.top = startP.yCoord;
    boundaryBoxPixel.right = endP.xCoord;
    boundaryBoxPixel.bottom = endP.yCoord;

    // UI에 표시 좌표 업데이트.
    colorKeyAndMouseUI.UpdateSelectBox(boundaryBoxPixel);
}

void App::SelectPivot()
{
    std::pair<int, int> pos = wnd.GetSpriteCoord(
        wnd.pivotSelectMouse.GetCurRawX(),
        wnd.pivotSelectMouse.GetCurRawY(),
        pSpriteSheet->GetBitmapSize());

    // x좌표를 현재 선택한 프레임 안의 범위로 한정함.
    if (pos.xCoord < framesInProcess[currentAnimInfoUI.currentFrameIndex].left) pos.xCoord = framesInProcess[currentAnimInfoUI.currentFrameIndex].left;
    if (pos.xCoord > framesInProcess[currentAnimInfoUI.currentFrameIndex].right) pos.xCoord = framesInProcess[currentAnimInfoUI.currentFrameIndex].right;
    // y좌표는 현재 선택한 프레임 좌표의 bottom 좌표로 한정함.
    pos.yCoord = framesInProcess[currentAnimInfoUI.currentFrameIndex].bottom;

    pivotPos = D2D1_POINT_2L{ pos.xCoord, pos.yCoord };
    // UI에 표시 좌표 업데이트
    colorKeyAndMouseUI.UpdatePivotPoint(pivotPos);
}
#pragma endregion

#pragma region MakeStuff
void App::MakeFrame()
{
    D2D1_RECT_L frameCoord;
    frameCoord = pSpriteSheet->CheckPixelBoundArea(
        D2D1_RECT_L{ boundaryBoxPixel.left, boundaryBoxPixel.top, boundaryBoxPixel.right, boundaryBoxPixel.bottom }
    );

    if (wnd.boundarySelectMouse.isClicked == false)
        // 애니메이션 프레임에 넣어주기.
        framesInProcess.push_back(Frame{ 0,0, frameCoord.left, frameCoord.top, frameCoord.right, frameCoord.bottom });
}

void App::MakePivot()
{
    framesInProcess[currentAnimInfoUI.currentFrameIndex].originX = pivotPos.x;
    framesInProcess[currentAnimInfoUI.currentFrameIndex].originY = pivotPos.y;
}
#pragma endregion


