#include "application/node_editor.h"

void NodeEditor::Draw() noexcept
{
	// Transparent background for the node editor
	ImNodesStyle& style = ImNodes::GetStyle();
	style.Colors[ImNodesCol_GridBackground] = IM_COL32(255, 255, 255, 25);

	ImGui::Begin("Scene Graph");
	{
		ImNodes::BeginNodeEditor();

		static const int id = 0;

		ImNodes::BeginNode(id);
		ImGui::Dummy(ImVec2(80.0f, 40.0f));
		ImNodes::EndNode();

		ImNodes::EndNodeEditor();
	}
	ImGui::End();
}