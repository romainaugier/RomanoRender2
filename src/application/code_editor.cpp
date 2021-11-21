#include "application/code_editor.h"

void CodeEditor::Draw() noexcept
{
	ImGui::Begin("Code Editor");
	{
		ImGui::Text("Shader Name");

		static char codeBuffer[8192];

		const float width = ImGui::GetWindowContentRegionWidth();
		static const ImVec2 textEditorSize = ImVec2(width, 350.0f);

		if (ImGui::InputTextMultiline("Code", codeBuffer, 8192, textEditorSize)) this->isBeingEdited = true;
		else this->isBeingEdited = false;
		
		if (ImGui::Button("Compile"))
		{

		}

		ImGui::SameLine();
		ImGui::Button("Save");
	}
	ImGui::End();
}