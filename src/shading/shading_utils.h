#pragma once

#include "shading/shading.h"
#include "renderer/renderer.h"
#include "utils/logger.h"

#include "OSL/oslcomp.h"

namespace utils
{
	// Function to add a shader (as a string buffer) to the oslShadingSystem
	bool AddShader(RomanoRenderer& renderer,
				   const uint32_t shaderID,
				   std::string& shaderBuffer,
				   std::string shaderName) noexcept;

	// Function to update an existing shader of the oslShadingSystem
	bool UpdateShader(RomanoRenderer& renderer,
					  const uint32_t shaderID,
					  std::string& shaderBuffer,
					  std::string& shaderName) noexcept;

	// Function to update the exposed parameters of an existing shader
	void UpdateShaderParams() noexcept;
}