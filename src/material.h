#pragma once

#ifndef MATERIAL
#define MATERIAL

#include "common/math/vec3.h"
#include <stdint.h>

struct Material
{
	embree::Vec3f color;

	uint32_t id;

	Material() {}

	Material(const embree::Vec3f col, const uint32_t id) : 
		color(col),
		id(id) {}
};

#endif