#include "ImUI_CurretAnimInfo.h"


ImUI_CurretAnimInfo::ImUI_CurretAnimInfo(Window& wnd)
	:
	ImUI(wnd),
	currentFrameIndex(-1),
	currentAnimIndex(-1)
{
}

void ImUI_CurretAnimInfo::Run(std::vector<SpriteAnimation>& anims, wchar_t* fileName)
{
	ImGui::Begin("Frame Info", &showUI);
	{
#pragma region ComboBox
		std::vector<std::string> names;
		for (int i = 0; i < anims.size(); ++i)
		{
			CW2A cw2a(anims[i].animationName, CP_UTF8);
			names.push_back(cw2a.m_psz);
		}

		static std::string current_item;

		if (ImGui::BeginCombo("##combo", current_item.c_str()))
		{
			for (int n = 0; n < anims.size(); ++n)
			{
				is_selected = (current_item == names[n]);
				if (ImGui::Selectable(names[n].c_str(), is_selected))
				{
					current_item = names[n];
					currentAnimIndex = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
#pragma endregion

#pragma region Table
		// 프레임 선택 박스 좌표 표시 테이블
		ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable;

		// When using ScrollX or ScrollY we need to specify a size for our table container!
		// Otherwise by default the table will fit all available space, like a BeginChild() call.
		const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
		ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);

		if (ImGui::BeginTable("Frame Boundaries", 4, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("Frame", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Boundary(Abs)", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Pivot(Rel)", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Collision(Rel)", ImGuiTableColumnFlags_None);
			ImGui::TableHeadersRow();

			int size = 0;
			if ((anims.size() > 0) && ((size = anims[currentAnimIndex].frames.size()) > 0))
			{
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
					sprintf_s(frame, "(l:%d, t:%d, r:%d, b:%d)", 
						anims[currentAnimIndex].frames[i].left,
						anims[currentAnimIndex].frames[i].top,
						anims[currentAnimIndex].frames[i].right,
						anims[currentAnimIndex].frames[i].bottom);
					ImGui::Text(frame);
					ImGui::TableNextColumn();
					sprintf_s(frame, "(%d, %d)", anims[currentAnimIndex].frames[i].originX, anims[currentAnimIndex].frames[i].originY);
					ImGui::Text(frame);
					ImGui::TableNextColumn();
					for (int j = 0; j < anims[currentAnimIndex].frames[i].nCollisions; ++j)
					{
						sprintf_s(frame, "(l:%d, t:%d, r:%d, b:%d)",
							anims[currentAnimIndex].frames[i].collisions[j].left,
							anims[currentAnimIndex].frames[i].collisions[j].top,
							anims[currentAnimIndex].frames[i].collisions[j].right,
							anims[currentAnimIndex].frames[i].collisions[j].bottom);
						ImGui::Text(frame);
					}
					ImGui::TableNextRow();
				}
			}
			ImGui::EndTable();
		}
#pragma endregion

#pragma region Anim Info and Create Anim
		wchar_t info[32];

		if (anims.size() > 0)
		{
			StringCbPrintf(info, sizeof(info), L"> 현재 선택한 애니메이션 : %s", anims[currentAnimIndex].animationName);
			ImGui::Text(WideToMultiU8(info).c_str());
		}
		else
		{
			StringCbPrintf(info, sizeof(info), L"> 현재 선택한 애니메이션 : 없음.");
			ImGui::Text(WideToMultiU8(info).c_str());
		}

		if (currentFrameIndex >= 0)
		{
			StringCbPrintf(info, sizeof(info), L"> 현재 선택한 프레임 : %d", currentFrameIndex);
			ImGui::Text(WideToMultiU8(info).c_str());
		}
		else
		{
			StringCbPrintf(info, sizeof(info), L"> 현재 선택한 프레임 : 없음.");
			ImGui::Text(WideToMultiU8(info).c_str());
		}

		if (ImGui::Button(WideToMultiU8(L"애니메이션 생성").c_str()))
			bAddAnimation = true;
		if (bAddAnimation)
		{
			ImGui::InputText("##animation name", buf, sizeof(buf));
			ImGui::SameLine();
			if (ImGui::Button(WideToMultiU8(L"생성").c_str()))
			{
				if (strcmp(buf, "") != 0)
				{
					SpriteAnimation spra;
					StringCbPrintf(spra.imageSourceName, sizeof(spra.imageSourceName), fileName);
					StringCbPrintf(spra.animationName, sizeof(spra.animationName), MultiU8ToWide(buf).c_str());
					spra.nFrames = 0;
					spra.colorKey = 0xffffffff;
					spra.frames = std::vector<Frame>{};
					anims.push_back(spra);
					currentAnimIndex++;
					bAddAnimation = false;
				}
			}
		}
	}
#pragma endregion

	ImGui::End();
}
