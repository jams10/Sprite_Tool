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
    // ��������Ʈ ��Ʈ �̵��� ���� ���콺 ��ǥ ������Ʈ.
    if (wnd.moveMouse.isClicked)
    {
        sheetPos.x += (wnd.moveMouse.GetCurRawX() - wnd.moveMouse.GetLastRawX());
        sheetPos.y += (wnd.moveMouse.GetCurRawY() - wnd.moveMouse.GetLastRawY());
        wnd.moveMouse.SetLastCoord(wnd.moveMouse.GetCurRawX(), wnd.moveMouse.GetCurRawY());
    }

    if (pSpriteSheet != nullptr)
    {
        // 1. Color Key ����.
        if (wnd.bColorKeySelectMode && wnd.colorKeySelectMouse.isClicked)
            SelectColorKey();
        // 2. ������ ��� ���� �� �߰�.
        else if (wnd.bBoundarySelectMode && wnd.boundarySelectMouse.isClicked)
            SelectArea();
        if (wnd.bAddFrame)
        {
            MakeFrame();
            wnd.bAddFrame = false;
        }
        // 3. ������ �Ǻ� ������ ���� �� �߰�.
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

    // ��������Ʈ ��Ʈ ��ȯ ��� ������Ʈ.
    wnd.D2D().UpdateMouseMatrix(sheetPos.x, sheetPos.y);
    wnd.D2D().UpdateScaleMatrix(wnd.scale);

}

void App::Render()
{
    wnd.D3D().BeginFrame();
    wnd.D2D().BeginFrame();

    // 1. ��������Ʈ ��Ʈ �׸���.
    DrawSpriteSheet();
    // 2. �ȼ� ��� üũ�� ���� �ڽ� �׸���.
    if(wnd.bBoundarySelectMode)
        DrawBoundarySelectBox(boundaryBoxScreen);
    // 3. �Ǻ� ���� ���� �׸���.
    if (wnd.bPivotSelectMode && currentAnimInfoUI.currentFrameIndex >= 0)
    {
        DrawPivotPoint();
    }

    // �۾��� ���� ������ ���� ������ �׸���.
    DrawMadeFrames();

    // UI �׸���
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

    // ������ ������ �κ��� �ٽ� ǥ������ �ʱ� ���� �ʱ�ȭ.
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
        // ��ũ�� ��ǥ�� ��ȯ �Ͽ� ������ ���� �ڽ� �׸���.
        wnd.D2D().DrawBox(D2D1_RECT_F(
            Mouse::GetScreenFromPixel(f.left, pSpriteSheet->GetBitmapSize().width),
            Mouse::GetScreenFromPixel(f.top, pSpriteSheet->GetBitmapSize().height),
            Mouse::GetScreenFromPixel(f.right, pSpriteSheet->GetBitmapSize().width),
            Mouse::GetScreenFromPixel(f.bottom, pSpriteSheet->GetBitmapSize().height)
        ));
        wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 0.f,0.f,1.f,0.5f });
        // ��ũ�� ��ǥ�� ��ȯ�Ͽ� ���� pivot �� �׸���.
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

    // �÷� Ű ������Ʈ.
    DWORD color = pSpriteSheet->GetPixelColor(colorKeyPos.xCoord, colorKeyPos.yCoord);
    pSpriteSheet->SetPixelColorKey(color);
    // UI ǥ�� ���� ������Ʈ.
    colorKeyAndMouseUI.UpdateColorKey(color);
}

void App::SelectArea()
{
    // ���� ���� ��ǥ ������Ʈ. (��ũ�� �߾��� ������ ��ǥ)
    std::pair<float, float> startS = wnd.GetScreenCoord(wnd.boundarySelectMouse.GetLastRawX(), wnd.boundarySelectMouse.GetLastRawY());
    std::pair<float, float> endS = wnd.GetScreenCoord(wnd.boundarySelectMouse.GetCurRawX(), wnd.boundarySelectMouse.GetCurRawY());
    boundaryBoxScreen.left = startS.xCoord;
    boundaryBoxScreen.top = startS.yCoord;
    boundaryBoxScreen.right = endS.xCoord;
    boundaryBoxScreen.bottom = endS.yCoord;

    // ���� ���� �ȼ� ��ǥ.
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

    // UI�� ǥ�� ��ǥ ������Ʈ.
    colorKeyAndMouseUI.UpdateSelectBox(boundaryBoxPixel);
}

void App::SelectPivot()
{
    std::pair<int, int> pos = wnd.GetSpriteCoord(
        wnd.pivotSelectMouse.GetCurRawX(),
        wnd.pivotSelectMouse.GetCurRawY(),
        pSpriteSheet->GetBitmapSize());

    // x��ǥ�� ���� ������ ������ ���� ������ ������.
    if (pos.xCoord < framesInProcess[currentAnimInfoUI.currentFrameIndex].left) pos.xCoord = framesInProcess[currentAnimInfoUI.currentFrameIndex].left;
    if (pos.xCoord > framesInProcess[currentAnimInfoUI.currentFrameIndex].right) pos.xCoord = framesInProcess[currentAnimInfoUI.currentFrameIndex].right;
    // y��ǥ�� ���� ������ ������ ��ǥ�� bottom ��ǥ�� ������.
    pos.yCoord = framesInProcess[currentAnimInfoUI.currentFrameIndex].bottom;

    pivotPos = D2D1_POINT_2L{ pos.xCoord, pos.yCoord };
    // UI�� ǥ�� ��ǥ ������Ʈ
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
        // �ִϸ��̼� �����ӿ� �־��ֱ�.
        framesInProcess.push_back(Frame{ 0,0, frameCoord.left, frameCoord.top, frameCoord.right, frameCoord.bottom });
}

void App::MakePivot()
{
    framesInProcess[currentAnimInfoUI.currentFrameIndex].originX = pivotPos.x;
    framesInProcess[currentAnimInfoUI.currentFrameIndex].originY = pivotPos.y;
}
#pragma endregion


