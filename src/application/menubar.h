#pragma once

#include "imgui.h"

#include "renderer/renderer.h"

struct MenuBar
{
	RomanoRenderer* renderer;

	void Draw() noexcept;
};