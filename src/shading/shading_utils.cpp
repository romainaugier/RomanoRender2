#include "shading/shading_utils.h"

namespace utils
{
	bool AddShader(RomanoRenderer& renderer,
				   const uint32_t shaderID,
				   std::string& shaderBuffer,
				   std::string shaderName) noexcept
	{
		OSL::OSLCompiler compiler(&renderer.oslErrHandler);
		std::string osoOutput;
		std::vector<std::string> options;

		bool hasCompiled = compiler.compile_buffer(shaderBuffer, osoOutput, options);

		if (hasCompiled)
		{
			renderer.logger->Log(LogLevel_Diagnostic, "Shader %s compiled successfully.", shaderName.c_str());

			bool addedToShadingSys = renderer.oslShadingSys->LoadMemoryCompiledShader(shaderName, osoOutput);

			if (addedToShadingSys)
			{
				renderer.logger->Log(LogLevel_Diagnostic, "Shader %s added to OSL Shading System successfully", shaderName.c_str());

				OSL::ShaderGroupRef group = renderer.oslShadingSys->ShaderGroupBegin();
				renderer.oslShadingSys->Shader("surface", shaderName, "layer1");
				renderer.oslShadingSys->ShaderGroupEnd();

				renderer.sceneShaders.insert(std::make_pair(shaderID, std::make_pair(shaderName, group)));
			
				renderer.numShader++;
			}
		}
		else
		{
			renderer.logger->Log(LogLevel_Error, "Failed to compiler shader %s", shaderName.c_str());
		}
	}

	bool UpdateShader(RomanoRenderer& renderer,
					  const uint32_t shaderID,
					  std::string& shaderBuffer,
					  std::string& shaderName) noexcept
	{
		OSL::OSLCompiler compiler(&renderer.oslErrHandler);
		std::string osoOutput;
		std::vector<std::string> options;

		bool hasCompiled = compiler.compile_buffer(shaderBuffer, osoOutput, options);

		if (hasCompiled)
		{
			renderer.logger->Log(LogLevel_Diagnostic, "Shader %s compiled successfully.", shaderName.c_str());

			bool addedToShadingSys = renderer.oslShadingSys->LoadMemoryCompiledShader(shaderName, osoOutput);

			if (addedToShadingSys)
			{
				renderer.logger->Log(LogLevel_Diagnostic, "Shader %s added to OSL Shading System successfully", shaderName.c_str());
				
				renderer.sceneShaders[shaderID].second = renderer.oslShadingSys->ShaderGroupBegin();
				renderer.oslShadingSys->Shader("surface", shaderName, "layer1");
				renderer.oslShadingSys->ShaderGroupEnd();

				renderer.logger->Log(LogLevel_Diagnostic, "Updated shader %s successfully", shaderName.c_str());
			}
		}
		else
		{
			renderer.logger->Log(LogLevel_Error, "Failed to compiler shader %s", shaderName.c_str());
		}
	}

	void UpdateShaderParams() noexcept
	{
		
	}
}