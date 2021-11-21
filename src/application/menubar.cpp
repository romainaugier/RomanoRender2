#include "application/menubar.h"

void MenuBar::Draw() noexcept
{
	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import Obj"))
			{

			}

			if (ImGui::MenuItem("Import OSL Shader"))
			{

			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}

		const float availHeight = ImGui::GetContentRegionAvail().y;
		float availWidth = ImGui::GetContentRegionAvail().x;

		ImGui::Dummy(ImVec2(availWidth - 400.0f, availHeight));

		ImGui::Text("Samples : %d", this->renderer->numSamples);

		ImGui::Dummy(ImVec2(20.0f, availHeight));

		ImGui::Text("Progress : %d%%", 0);

		ImGui::Dummy(ImVec2(20.0f, availHeight));

		ImGui::Button("Render");

		ImGui::Dummy(ImVec2(20.0f, availHeight));

		availWidth = ImGui::GetContentRegionAvail().x;

		static const char* items[] = {"Menu", "Ocio"};
		static int currentItem = 0;

		ImGui::SetNextItemWidth(60.0f);
		ImGui::Combo("", &currentItem, &items[0], IM_ARRAYSIZE(items));
	}

	ImGui::EndMainMenuBar();
}