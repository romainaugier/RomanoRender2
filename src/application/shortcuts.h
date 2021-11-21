#pragma once

#include "imgui.h"

#include <unordered_map>

using ShortcutsMap = std::unordered_map<std::string, bool>;

struct Shortcuts
{
	ShortcutsMap map;

	Shortcuts();

	void Process() noexcept;
};