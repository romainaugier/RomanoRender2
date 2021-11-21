#pragma once

#include "imgui.h"

#include "renderer/renderer.h"
#include "shading/shading_utils.h"

struct CodeEditor
{
	RomanoRenderer* renderer = nullptr;
	bool isBeingEdited = false;

	void Draw() noexcept;
};