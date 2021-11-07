#pragma once

#ifndef SPHERE
#define SPHERE

#include "boundingbox.h"
#include "ispc/intersectors.h"
#include <algorithm>

struct alignas(32) Sphere
{
	vec3 center;

	float radius;

	uint32_t id = 0;
	uint32_t matId = 0;

	uint32_t padding[2]; // ensure 32 bytes size

	Sphere(vec3 _center, float _radius, unsigned int _id, unsigned int _mat_id) :
		center(_center),
		radius(_radius),
		id(_id),
		matId(_mat_id) {}
};

bool SphereHit(const Sphere& sphere, RTCRayHit& rayhit) noexcept;
bool SphereHitN(const Sphere* spheres, RTCRayHit& rayhit, const int count) noexcept;
bool SphereOcclude(const Sphere& sphere, RTCRay& shadow) noexcept;
bool SphereOccludeN(const Sphere* spheres, RTCRay& shadow, const int count) noexcept;


BoundingBox SphereBBox(const Sphere& sphere) noexcept;

#endif // !SPHERE

