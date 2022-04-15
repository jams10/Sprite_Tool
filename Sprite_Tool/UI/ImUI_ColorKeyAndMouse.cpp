#include "ImUI_ColorKeyAndMouse.h"

ImUI_ColorKeyAndMouse::ImUI_ColorKeyAndMouse(Window& wnd)
	:
	ImUI(wnd),
	colorKey(0xffffffff),
	selectBox(D2D1_RECT_L{0,0,0,0}),
	pivotPoint(D2D1_POINT_2L(0,0))
{
}

void ImUI_ColorKeyAndMouse::Run()
{
	ImGui::Begin("Color Key & Mouse Info", &showUI);
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Text("Color Key Select Mode : %s", wnd.bColorKeySelectMode ? "ON" : "OFF"); ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button,
			ImVec4{ ((colorKey & 0x00ff0000) >> 16) / 255.0f,
				   ((colorKey & 0x0000ff00) >> 8) / 255.0f,
				   ((colorKey & 0x000000ff)) / 255.0f,
				   ((colorKey & 0xff000000) >> 24) / 255.0f });
		ImGui::Button(" ", ImVec2{ 12,12 });
		ImGui::PopStyleColor(1);

		ImGui::Text("Pivot Select Mode : %s", wnd.bPivotSelectMode ? "ON" : "OFF");

		ImGui::Text("Select Box Coord : start(%d, %d) / end(%d, %d)", selectBox.left, selectBox.top, selectBox.right, selectBox.bottom);
		ImGui::Text("Select Pivot Coord : (%d, %d)", pivotPoint.x, pivotPoint.y);
	}
	ImGui::End();
}
