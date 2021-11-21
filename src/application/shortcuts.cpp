#include "application/shortcuts.h"

Shortcuts::Shortcuts()
{
	// Set all the shortcuts to false when initializing the shortcuts struct
	this->map["space"] = false;
}

void Shortcuts::Process() noexcept
{
	// Space key
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
	{
		if (this->map["space"]) this->map["space"] = false;
		else this->map["space"] = true;
	}
}
