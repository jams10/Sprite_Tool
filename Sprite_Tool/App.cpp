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
    currentAnimInfoUI(wnd),
    displayAnimUI(wnd),
    elapsedTime(0.f),
    holdTime(0.2f),
    currentSelectAnimIndex(-1),
    currentSelectFrameIndex(-1),
    currentAnimFrameIndex(0)

{
    anims.reserve(20);
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
    elapsedTime += timer.Mark();
    holdTime = displayAnimUI.framespeed;

    currentSelectAnimIndex = currentAnimInfoUI.currentAnimIndex;
    currentSelectFrameIndex = currentAnimInfoUI.currentFrameIndex;

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
        if (wnd.bBoundarySelectMode && wnd.boundarySelectMouse.isClicked )
            SelectArea(wnd.boundarySelectMouse, boundaryBoxScreen, boundaryBoxPixel);
        if (wnd.bAddFrame && currentSelectAnimIndex >= 0)
        {
            MakeFrame();
            wnd.bAddFrame = false;
        }
        // 3. 프레임 피봇 기준점 선택 및 추가.
        if (wnd.bPivotSelectMode && currentAnimInfoUI.currentFrameIndex >= 0 )
        {
            SelectPivot();
        }
        if (wnd.bAddPivot)
        {
            MakePivot();
            wnd.bAddPivot = false;
        }
        // 4. 콜리젼 영역 선택 및 추가.
        if (wnd.bCollisionSelectMode && wnd.collisionSelectMouse.isClicked && currentAnimInfoUI.currentFrameIndex >= 0)
        {
            SelectArea(wnd.collisionSelectMouse, collisionBoxScreen, collisionBoxPixel);
        }
        if (wnd.bAddCollision)
        {
            MakeCollision();
            wnd.bAddCollision = false;
        }
    }

    // 스프라이트 시트 변환 행렬 업데이트.
    wnd.D2D().UpdateMouseMatrix(sheetPos.x, sheetPos.y);
    wnd.D2D().UpdateScaleMatrix(wnd.scale);

    // 애니메이션 업데이트
    if (currentSelectAnimIndex >= 0 && anims[currentSelectAnimIndex].frames.size() > 0 && elapsedTime >= holdTime)
    {
        elapsedTime = 0.f;
        if (currentAnimFrameIndex >= anims[currentSelectAnimIndex].frames.size()) currentAnimFrameIndex = 0;
        wnd.D3D().UpdateTextureForAnimation(
            anims[currentSelectAnimIndex].frames[currentAnimFrameIndex], 
            pSpriteSheet, 
            displayAnimUI.heightOffset);
        currentAnimFrameIndex++;
    }
}

void App::Render()
{
    wnd.D3D().BeginFrame();
    wnd.D2D().BeginFrame();

    // 1. 스프라이트 시트 그리기.
    DrawSpriteSheet();
    // 2. 픽셀 경계 체크용 선택 박스 그리기.
    if(wnd.bBoundarySelectMode)
        DrawSelectBox(boundaryBoxScreen, D2D1_COLOR_F{1.f, 1.f, 1.f, 1.f});
    // 3. 피봇 선택 지점 그리기.
    if (wnd.bPivotSelectMode && currentAnimInfoUI.currentFrameIndex >= 0)
    {
        DrawPivotPoint();
    }
    // 4. 콜리젼 선택 지점 그리기.
    if (wnd.bCollisionSelectMode)
        DrawSelectBox(collisionBoxScreen, D2D1_COLOR_F{0.f, 1.f, 1.f, 1.f});

    // 작업이 끝난 프레임 선택 지점들 그리기.
    if(anims.size() > 0)
        DrawMadeFrames();

    // UI 그리기
    if(anims.size() == 0)
        fileUI.Run(&pSpriteSheet, SpriteAnimation{});
    else
        fileUI.Run(&pSpriteSheet, anims[currentSelectAnimIndex]);
    colorKeyAndMouseUI.Run();
    currentAnimInfoUI.Run(anims, fileUI.GetFileName());
    displayAnimUI.Run(wnd.D3D().GetShaderResourceView(), wnd.D3D().GetAnimTextureWidth(), wnd.D3D().GetAnimTextureHeight());

    wnd.D2D().EndFrame();
    wnd.D3D().EndFrame();
}

#pragma region DrawStuff
void App::DrawSpriteSheet()
{
    if(pSpriteSheet)
        wnd.D2D().DrawBitmap(*pSpriteSheet->GetD2DBitmap());
}

void App::DrawSelectBox(D2D1_RECT_F& selectBox, const D2D1_COLOR_F& color)
{
    wnd.D2D().SetBrushColor(color);
    wnd.D2D().DrawBox(selectBox);

    // 이전에 선택한 부분을 다시 표시하지 않기 위해 초기화.
    selectBox = D2D1_RECT_F{ -1.f, -1.f, -1.f, -1.f };
}

void App::DrawPivotPoint()
{
    wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 0.f,0.f,1.f,1.f });

    float x = Mouse::GetScreenFromPixel(pivotPos.x, pSpriteSheet->GetBitmapSize().width);
    float y1 = Mouse::GetScreenFromPixel(anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].top, pSpriteSheet->GetBitmapSize().height);
    float y2 = Mouse::GetScreenFromPixel(anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].bottom, pSpriteSheet->GetBitmapSize().height);

    wnd.D2D().DrawLine(D2D1_POINT_2F{ x, y1 }, D2D1_POINT_2F{ x, y2 });
}

void App::DrawMadeFrames()
{
    for (const Frame& f : anims[currentSelectAnimIndex].frames)
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
        // 스크린 좌표로 변환하여 선택한 pivot 선 그리기.
        wnd.D2D().DrawLine(
            D2D1_POINT_2F(
                Mouse::GetScreenFromPixel(f.left + f.originX, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.top, pSpriteSheet->GetBitmapSize().height)),
            D2D1_POINT_2F(
                Mouse::GetScreenFromPixel(f.left + f.originX, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.bottom, pSpriteSheet->GetBitmapSize().height)
            ));
        wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 0.f, 1.f, 1.f, 0.5f });
        // 스크린 좌표로 변환하여 선택한 콜리젼 박스 그리기.
        for (int i = 0; i < f.nCollisions; ++i)
        {
            wnd.D2D().DrawBox(D2D1_RECT_F(
                Mouse::GetScreenFromPixel(f.left + f.originX + f.collisions[i].left, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.top + f.originY + f.collisions[i].top, pSpriteSheet->GetBitmapSize().height),
                Mouse::GetScreenFromPixel(f.left + f.originX + f.collisions[i].right, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.top + f.originY + f.collisions[i].bottom, pSpriteSheet->GetBitmapSize().height)
            ));
        }
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
    // 현재 작업중인 애니메이션의 컬러 키 값 업데이트.
    if (currentSelectAnimIndex >= 0)
        anims[currentSelectAnimIndex].colorKey = color;
    // UI 표시 색상 업데이트.
    colorKeyAndMouseUI.UpdateColorKey(color);
}

void App::SelectArea(const Mouse& mouse, D2D1_RECT_F& screen, D2D1_RECT_L& pixel)
{
    // 선택 영역 좌표 업데이트. (스크린 중앙이 원점인 좌표)
    std::pair<float, float> startS = wnd.GetScreenCoord(mouse.GetLastRawX(), mouse.GetLastRawY());
    std::pair<float, float> endS = wnd.GetScreenCoord(mouse.GetCurRawX(), mouse.GetCurRawY());
    screen.left = startS.xCoord;
    screen.top = startS.yCoord;
    screen.right = endS.xCoord;
    screen.bottom = endS.yCoord;

    // 선택 영역 픽셀 좌표.
    std::pair<int, int> startP = wnd.GetSpriteCoord(
        mouse.GetLastRawX(),
        mouse.GetLastRawY(),
        pSpriteSheet->GetBitmapSize());
    std::pair<int, int> endP = wnd.GetSpriteCoord(
        mouse.GetCurRawX(),
        mouse.GetCurRawY(),
        pSpriteSheet->GetBitmapSize());
    pixel.left = startP.xCoord;
    pixel.top = startP.yCoord;
    pixel.right = endP.xCoord;
    pixel.bottom = endP.yCoord;

    // UI에 표시 좌표 업데이트.
    colorKeyAndMouseUI.UpdateSelectBox(pixel);
}

void App::SelectPivot()
{
    std::pair<int, int> pos = wnd.GetSpriteCoord(
        wnd.pivotSelectMouse.GetCurRawX(),
        wnd.pivotSelectMouse.GetCurRawY(),
        pSpriteSheet->GetBitmapSize());

    // x좌표를 현재 선택한 프레임 안의 범위로 한정함.
    
    if (pos.xCoord < anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].left) pos.xCoord = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].left;
    if (pos.xCoord > anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].right) pos.xCoord = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].right;
    // y좌표는 현재 선택한 프레임 좌표의 bottom 좌표로 한정함.
    pos.yCoord = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].bottom;

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
    {
        // 애니메이션 프레임에 넣어주기.
        anims[currentSelectAnimIndex].frames.push_back(Frame{ 0,0, frameCoord.left, frameCoord.top, frameCoord.right, frameCoord.bottom });
        anims[currentSelectAnimIndex].nFrames = anims[currentSelectAnimIndex].frames.size();
        //framesInProcess.push_back(Frame{ 0,0, frameCoord.left, frameCoord.top, frameCoord.right, frameCoord.bottom });
    }
}

void App::MakePivot()
{
    anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].originX = 
        pivotPos.x - anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].left;
    anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].originY =
        pivotPos.y - anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].top;
}
void App::MakeCollision()
{
    D2D1_RECT_L collisionCoord;
    collisionCoord = pSpriteSheet->CheckPixelBoundArea(
        D2D1_RECT_L{ collisionBoxPixel.left, collisionBoxPixel.top, collisionBoxPixel.right, collisionBoxPixel.bottom }
    );

    // 현재 구해진 콜리젼 좌표가 절대 좌표이므로, 피봇 기준 상대좌표로 구하기 위해 피봇 좌표를 일단 절대 좌표로 만들어줌.
    int currentPivotX = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].left + anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].originX;
    int currentPivotY = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].top + anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].originY;
    // 피봇 좌표를 기준으로 얼마나 떨어져있는지 계산해 해당 값을 최종 좌표로 저장해줌.
    collisionCoord.left = collisionCoord.left - currentPivotX;
    collisionCoord.right = collisionCoord.right - currentPivotX;
    collisionCoord.top = collisionCoord.top - currentPivotY;
    collisionCoord.bottom = collisionCoord.bottom - currentPivotY;

    if (wnd.collisionSelectMouse.isClicked == false)
    {
        anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].collisions.push_back(Collision{ collisionCoord.left, collisionCoord.top, collisionCoord.right, collisionCoord.bottom });
        anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].nCollisions += 1;
    }
}
#pragma endregion


