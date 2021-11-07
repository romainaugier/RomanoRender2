#pragma once

#ifndef MATERIAL
#define MATERIAL

#include "vec3.h"
#include <stdint.h>

struct Material
{
	vec3 color;

	uint32_t id;

	Material() {}

	Material(const vec3 col, const uint32_t id) : 
		color(col),
		id(id) {}
};

#endif