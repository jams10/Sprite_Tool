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
        if (wnd.bBoundarySelectMode && wnd.boundarySelectMouse.isClicked )
            SelectArea(wnd.boundarySelectMouse, boundaryBoxScreen, boundaryBoxPixel);
        if (wnd.bAddFrame && currentSelectAnimIndex >= 0)
        {
            MakeFrame();
            wnd.bAddFrame = false;
        }
        // 3. ������ �Ǻ� ������ ���� �� �߰�.
        if (wnd.bPivotSelectMode && currentAnimInfoUI.currentFrameIndex >= 0 )
        {
            SelectPivot();
        }
        if (wnd.bAddPivot)
        {
            MakePivot();
            wnd.bAddPivot = false;
        }
        // 4. �ݸ��� ���� ���� �� �߰�.
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

    // ��������Ʈ ��Ʈ ��ȯ ��� ������Ʈ.
    wnd.D2D().UpdateMouseMatrix(sheetPos.x, sheetPos.y);
    wnd.D2D().UpdateScaleMatrix(wnd.scale);

    // �ִϸ��̼� ������Ʈ
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

    // 1. ��������Ʈ ��Ʈ �׸���.
    DrawSpriteSheet();
    // 2. �ȼ� ��� üũ�� ���� �ڽ� �׸���.
    if(wnd.bBoundarySelectMode)
        DrawSelectBox(boundaryBoxScreen, D2D1_COLOR_F{1.f, 1.f, 1.f, 1.f});
    // 3. �Ǻ� ���� ���� �׸���.
    if (wnd.bPivotSelectMode && currentAnimInfoUI.currentFrameIndex >= 0)
    {
        DrawPivotPoint();
    }
    // 4. �ݸ��� ���� ���� �׸���.
    if (wnd.bCollisionSelectMode)
        DrawSelectBox(collisionBoxScreen, D2D1_COLOR_F{0.f, 1.f, 1.f, 1.f});

    // �۾��� ���� ������ ���� ������ �׸���.
    if(anims.size() > 0)
        DrawMadeFrames();

    // UI �׸���
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

    // ������ ������ �κ��� �ٽ� ǥ������ �ʱ� ���� �ʱ�ȭ.
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
        // ��ũ�� ��ǥ�� ��ȯ �Ͽ� ������ ���� �ڽ� �׸���.
        wnd.D2D().DrawBox(D2D1_RECT_F(
            Mouse::GetScreenFromPixel(f.left, pSpriteSheet->GetBitmapSize().width),
            Mouse::GetScreenFromPixel(f.top, pSpriteSheet->GetBitmapSize().height),
            Mouse::GetScreenFromPixel(f.right, pSpriteSheet->GetBitmapSize().width),
            Mouse::GetScreenFromPixel(f.bottom, pSpriteSheet->GetBitmapSize().height)
        ));
        wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 0.f,0.f,1.f,0.5f });
        // ��ũ�� ��ǥ�� ��ȯ�Ͽ� ������ pivot �� �׸���.
        wnd.D2D().DrawLine(
            D2D1_POINT_2F(
                Mouse::GetScreenFromPixel(f.left + f.originX, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.top, pSpriteSheet->GetBitmapSize().height)),
            D2D1_POINT_2F(
                Mouse::GetScreenFromPixel(f.left + f.originX, pSpriteSheet->GetBitmapSize().width),
                Mouse::GetScreenFromPixel(f.bottom, pSpriteSheet->GetBitmapSize().height)
            ));
        wnd.D2D().SetBrushColor(D2D1_COLOR_F{ 0.f, 1.f, 1.f, 0.5f });
        // ��ũ�� ��ǥ�� ��ȯ�Ͽ� ������ �ݸ��� �ڽ� �׸���.
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

    // �÷� Ű ������Ʈ.
    DWORD color = pSpriteSheet->GetPixelColor(colorKeyPos.xCoord, colorKeyPos.yCoord);
    pSpriteSheet->SetPixelColorKey(color);
    // ���� �۾����� �ִϸ��̼��� �÷� Ű �� ������Ʈ.
    if (currentSelectAnimIndex >= 0)
        anims[currentSelectAnimIndex].colorKey = color;
    // UI ǥ�� ���� ������Ʈ.
    colorKeyAndMouseUI.UpdateColorKey(color);
}

void App::SelectArea(const Mouse& mouse, D2D1_RECT_F& screen, D2D1_RECT_L& pixel)
{
    // ���� ���� ��ǥ ������Ʈ. (��ũ�� �߾��� ������ ��ǥ)
    std::pair<float, float> startS = wnd.GetScreenCoord(mouse.GetLastRawX(), mouse.GetLastRawY());
    std::pair<float, float> endS = wnd.GetScreenCoord(mouse.GetCurRawX(), mouse.GetCurRawY());
    screen.left = startS.xCoord;
    screen.top = startS.yCoord;
    screen.right = endS.xCoord;
    screen.bottom = endS.yCoord;

    // ���� ���� �ȼ� ��ǥ.
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

    // UI�� ǥ�� ��ǥ ������Ʈ.
    colorKeyAndMouseUI.UpdateSelectBox(pixel);
}

void App::SelectPivot()
{
    std::pair<int, int> pos = wnd.GetSpriteCoord(
        wnd.pivotSelectMouse.GetCurRawX(),
        wnd.pivotSelectMouse.GetCurRawY(),
        pSpriteSheet->GetBitmapSize());

    // x��ǥ�� ���� ������ ������ ���� ������ ������.
    
    if (pos.xCoord < anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].left) pos.xCoord = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].left;
    if (pos.xCoord > anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].right) pos.xCoord = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].right;
    // y��ǥ�� ���� ������ ������ ��ǥ�� bottom ��ǥ�� ������.
    pos.yCoord = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].bottom;

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
    {
        // �ִϸ��̼� �����ӿ� �־��ֱ�.
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

    // ���� ������ �ݸ��� ��ǥ�� ���� ��ǥ�̹Ƿ�, �Ǻ� ���� �����ǥ�� ���ϱ� ���� �Ǻ� ��ǥ�� �ϴ� ���� ��ǥ�� �������.
    int currentPivotX = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].left + anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].originX;
    int currentPivotY = anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].top + anims[currentSelectAnimIndex].frames[currentSelectFrameIndex].originY;
    // �Ǻ� ��ǥ�� �������� �󸶳� �������ִ��� ����� �ش� ���� ���� ��ǥ�� ��������.
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


