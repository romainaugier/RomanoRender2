#pragma once

#include "shading/shading.h"
#include "utils/logger.h"

namespace utils
{
	// Function to add a shader (as a string buffer) to the oslShadingSystem
	void AddShader(OSL::ShadingSystem* oslShadingSys, 
				   std::string& shaderBuffer,
				   std::string shaderName) noexcept;

	// Function to update an existing shader of the oslShadingSystem
	void UpdateShader(OSL::ShadingSystem* oslShadingSys,
					  std::string& shaderBuffer) noexcept;

	// Function to update the exposed parameters of an existing shader
	void UpdateShaderParams() noexcept;
}