#pragma once

#ifndef GLUTILS
#define GLUTILS

#include "GL/glew.h"

class Shader
{
public:
	GLuint ID;

	Shader()
	{
		GLuint vertex, fragment;
		int success;
		char infoLog[512];

		// Vertex shader
		const char* vShaderCode = 
			"#version 330 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"layout(location = 1) in vec2 aTexCoord;\n"

			"out vec3 ourColor;\n"
			"out vec2 TexCoord;\n"

			"void main()\n"
			"{\n"
			"    gl_Position = vec4(aPos, 1.0);\n"
			"    TexCoord = aTexCoord;\n"
			"};\n";

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		// Fragment shader
		const char* fShaderCode =
			"#version 330 core\n"
			"out vec4 FragColor;\n"

			"in vec3 ourColor;\n"
			"in vec2 TexCoord;\n"

			"uniform sampler2D ourTexture;\n"

			"void main()\n"
			"{\n"
			"	gl_FragColor = texture(ourTexture, TexCoord);\n"
			"}\n";

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	inline void Use() const noexcept
	{
		glUseProgram(ID);
	}
};

//static void DrawBoundingBox(const BoundingBox& bbox, const vec3& color) noexcept
//{
//	glColor3f(color.x, color.y, color.z);
//
//	// +Y face
//	glVertex3f(bbox.p0.x, bbox.p1.y, bbox.p0.z);
//	glVertex3f(bbox.p1.x, bbox.p1.y, bbox.p0.z);
//	glVertex3f(bbox.p1.x, bbox.p1.y, bbox.p1.z);
//	glVertex3f(bbox.p0.x, bbox.p1.y, bbox.p1.z);
//
//	// +Z face
//	glVertex3f(bbox.p0.x, bbox.p1.y, bbox.p1.z);
//	glVertex3f(bbox.p1.x, bbox.p1.y, bbox.p1.z);
//	glVertex3f(bbox.p1.x, bbox.p0.y, bbox.p1.z);
//	glVertex3f(bbox.p0.x, bbox.p0.y, bbox.p1.z);
//
//	// +X face
//	glVertex3f(bbox.p1.x, bbox.p1.y, bbox.p1.z);
//	glVertex3f(bbox.p1.x, bbox.p0.y, bbox.p1.z);
//	glVertex3f(bbox.p1.x, bbox.p0.y, bbox.p0.z);
//	glVertex3f(bbox.p1.x, bbox.p0.y, bbox.p1.z);
//
//	// -Y face
//	glVertex3f(bbox.p0.x, bbox.p0.y, bbox.p0.z);
//	glVertex3f(bbox.p1.x, bbox.p0.y, bbox.p0.z);
//	glVertex3f(bbox.p1.x, bbox.p0.y, bbox.p1.z);
//	glVertex3f(bbox.p0.x, bbox.p0.y, bbox.p1.z);
//
//	// -Z face
//	glVertex3f(bbox.p0.x, bbox.p1.y, bbox.p0.z);
//	glVertex3f(bbox.p1.x, bbox.p1.y, bbox.p0.z);
//	glVertex3f(bbox.p1.x, bbox.p0.y, bbox.p0.z);
//	glVertex3f(bbox.p0.x, bbox.p0.y, bbox.p0.z);
//
//	// -X face
//	glVertex3f(bbox.p0.x, bbox.p1.y, bbox.p1.z);
//	glVertex3f(bbox.p0.x, bbox.p0.y, bbox.p1.z);
//	glVertex3f(bbox.p0.x, bbox.p0.y, bbox.p0.z);
//	glVertex3f(bbox.p0.x, bbox.p0.y, bbox.p1.z);
//}

#endif // !GLUTILS
