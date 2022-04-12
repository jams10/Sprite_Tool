#include "ImguiManager.h"
#include "../imgui/imgui.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();     // ���� üũ
	ImGui::CreateContext();   // Imgui ���̺귯���� ������ �۾��� �ϱ� ���� ���ؽ�Ʈ ����.
	ImGui::StyleColorsDark(); // ui �׸��� ���������� ��������.

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// �ѱ� ��Ʈ �߰�.
	io.Fonts->AddFontFromFileTTF("Fonts\\GimpoGothic\\Bold.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();  // ������ ���ؽ�Ʈ ����.
}