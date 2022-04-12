#include "ImUI_CurretAnimInfo.h"

ImUI_CurretAnimInfo::ImUI_CurretAnimInfo(Window& wnd)
	:
	ImUI(wnd),
	currentFrameIndex(-1)
{
}

// 선택한 프레임 인덱스 반환.
void ImUI_CurretAnimInfo::Run(const std::vector<Frame> frames)
{
	ImGui::Begin("Frame Info", &showUI);
	{

		// 프레임 선택 박스 좌표 표시 테이블
		ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable;

		// When using ScrollX or ScrollY we need to specify a size for our table container!
		// Otherwise by default the table will fit all available space, like a BeginChild() call.
		const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
		ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);

		if (ImGui::BeginTable("Frame Boundaries", 6, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("Frame", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Top", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Bottom", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Pivot", ImGuiTableColumnFlags_None);
			ImGui::TableHeadersRow();

			int size = frames.size();
			for (int i = 0; i < size; i++)
			{
				ImGui::TableNextColumn();
				char frame[32];
				sprintf_s(frame, "Frame : %d", i);
				if (ImGui::Button(frame))
				{
					currentFrameIndex = i;
				}
				ImGui::TableNextColumn();
				sprintf_s(frame, "%d", frames[i].left);
				ImGui::Text(frame);
				ImGui::TableNextColumn();
				sprintf_s(frame, "%d", frames[i].top);
				ImGui::Text(frame);
				ImGui::TableNextColumn();
				sprintf_s(frame, "%d", frames[i].right);
				ImGui::Text(frame);
				ImGui::TableNextColumn();
				sprintf_s(frame, "%d", frames[i].bottom);
				ImGui::Text(frame);
				ImGui::TableNextColumn();
				sprintf_s(frame, "(%d, %d)", frames[i].originX, frames[i].originY);
				ImGui::Text(frame);
				ImGui::TableNextRow();
			}
			ImGui::EndTable();
		}
		char frame[32];
		sprintf_s(frame, "Selected Frame : %d", currentFrameIndex);
		ImGui::Text(frame);
		ImGui::Button(WideToMultiU8(L"애니메이션 생성").c_str());
	}

	ImGui::End();
}
