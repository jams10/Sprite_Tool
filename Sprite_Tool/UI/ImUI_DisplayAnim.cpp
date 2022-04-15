#include "ImUI_DisplayAnim.h"

ImUI_DisplayAnim::ImUI_DisplayAnim(Window& wnd)
	:
	ImUI(wnd)
{
}

void ImUI_DisplayAnim::Run(ID3D11ShaderResourceView* srv, int width, int height)
{
	ImGui::Begin("Display Frame Animation");
	{
		if (ImGui::Button(WideToMultiU8(L"¾Ö´Ï¸ŞÀÌ¼Ç Àç»ı").c_str())) bPlayAnimation = true;

		if (bPlayAnimation)
		{
			//ImGui::Text("pointer = %p", srv);
			ImGui::Text("size = %d x %d", width, height);
			ImGui::Image((void*)srv, ImVec2(width, height)); // ÀÌ¹ÌÁö »çÀÌÁî¶û ¸ÂÃçÁà¾ß ÇÈ¼¿ ¾È±úÁü.

			ImGui::SliderFloat("frame speed", &framespeed, 0.f, 2.f, "%.1f", 1.f);
			ImGui::SliderFloat("height offset", &heightOffset, 0.1f, 1.0f, "%.1f", 1.f);
		}
	}
	ImGui::End();
}
