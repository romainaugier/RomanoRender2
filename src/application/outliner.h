#pragma once

#include "imgui.h"
#include "ImFileDialog.h"

#include "renderer/renderer.h"

#include <map>

// Simple struct that holds a shader representation for the interface
// We use this to store the shader code and its id 
// This mainly helps in linking the scenes shaders and the interface to
// edit them
struct ShaderHolder
{
	ShaderHolder(uint32_t newShaderID)
	{
		// Initialize the shader code with something simple

		this->code = "// OSL Shader\n"
			         "\n"
			         "surface\n"
			         "example\n"
			         "()\n"
			         "{\n"
			         "    Ci = color(0.5, 0.5, 0.5) * diffuse(N);\n"
			         "}";

		this->id = newShaderID;
	}

	std::string code;

	uint32_t id;
};

// Simple struct that holds an object reference representation, i.e its file path
// It will help holding all the objects imported when we import an .obj file
// This will change when implementing usd/alembic
struct ObjectHolder
{
	ObjectHolder(std::string filePath) 
	{
		this->filePath = filePath;
	}

	std::string filePath;
};

struct Outliner
{
	RomanoRenderer* renderer;

	std::map<std::string, ShaderHolder> shaders;
	uint32_t numShaders = 0;
	uint32_t numShadersIdIncr = 0;

	std::map<std::string, ObjectHolder> objects;

	void Draw() noexcept;
};