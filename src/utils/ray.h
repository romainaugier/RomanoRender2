#pragma once

#include "common/math/vec3.h"
#include "embree3/rtcore_ray.h"

#include "scene/camera.h"
#include "ispc/rand.h"

// Helper functions for rays
__forceinline void SetRayPosition(RTCRayHit& rayhit, const embree::Vec3f& position) noexcept { rayhit.ray.org_x = position.x; rayhit.ray.org_y = position.y; rayhit.ray.org_z = position.z; }
__forceinline void SetRayDirection(RTCRayHit& rayhit, const embree::Vec3f& direction) noexcept { rayhit.ray.dir_x = direction.x; rayhit.ray.dir_y = direction.y; rayhit.ray.dir_z = direction.z; }

__forceinline void SetRay(RTCRayHit& rayhit, const embree::Vec3f& position, const embree::Vec3f& direction, float t) noexcept
{
	SetRayPosition(rayhit, position);
	SetRayDirection(rayhit, direction);
	rayhit.ray.tnear = 0.0001f;
	rayhit.ray.tfar = t;
	rayhit.ray.mask = -1;
	rayhit.ray.flags = 0;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
}

__forceinline void SetPrimaryRay(RTCRayHit& rayhit, 
								 const Camera& cam,
								 const uint32_t x,
								 const uint32_t y,
								 const uint32_t xres,
								 const uint32_t yres,
	  							 const uint64_t sample) noexcept
{
	// Generate random numbers
	constexpr unsigned int floatAddr = 0x2f800004u;
	auto toFloat = float();
	memcpy(&toFloat, &floatAddr, 4);

	float randoms[2];
	int seeds[2] = { x * y * sample + 313, x * y * sample + 432 };
	ispc::randomFloatWangHash(seeds, randoms, toFloat, 2);

	// Generate xyz screen to normalized world coordinates

	// Very simple antialiasing
	// const float dx = embree::lerp(-0.5f, 0.5f, randoms[0]);
	// const float dy = embree::lerp(-0.5f, 0.5f, randoms[1]);
	
	// or
	
	const float dy = embree::sqrt(-0.5 * embree::log(randoms[1])) * embree::sin(2.0 * float(embree::pi) * randoms[2]);
	const float dx = embree::sqrt(-0.5 * embree::log(randoms[1])) * embree::cos(2.0 * float(embree::pi) * randoms[2]);

	const float xScreen = (2.0f * (x + dx) / float(xres) - 1.0f) * cam.aspect * cam.scale;
	const float yScreen = (1.0f - 2.0f * (y + dy) / float(yres)) * cam.scale;
	const float zScreen = -1.0f;

	// Transform to camera coordinates
	const embree::Vec3f rayDir(xScreen, yScreen, zScreen);
	const embree::Vec3f zero(embree::zero);
	const embree::Vec3f rayPosWorld = transform(zero, cam.transformation_matrix);
	const embree::Vec3f rayDirWorld = transform(rayDir, cam.transformation_matrix);
	const embree::Vec3f rayDirNorm = embree::normalize(rayDirWorld - rayPosWorld);

	SetRay(rayhit, cam.pos, rayDirNorm, 10000.0f);
}

__forceinline embree::Vec3f GetRayHitPosition(RTCRayHit& rayhit) noexcept
{
	return embree::Vec3f(rayhit.ray.org_x + rayhit.ray.dir_x * rayhit.ray.tfar,
		rayhit.ray.org_y + rayhit.ray.dir_y * rayhit.ray.tfar,
		rayhit.ray.org_z + rayhit.ray.dir_z * rayhit.ray.tfar);
}

__forceinline embree::Vec3f GetRayHitNormal(RTCRayHit& rayhit) noexcept { return embree::Vec3f(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z); }

__forceinline void PostIntersectCleanup(RTCRayHit& rayhit) noexcept
{
	const embree::Vec3f hitN = GetRayHitNormal(rayhit);
	const embree::Vec3f hitP = GetRayHitPosition(rayhit);

	SetRay(rayhit, hitP, hitN, 10000.0f);
}

__forceinline void PostIntersectPrep(RTCRayHit& rayhit, const embree::Vec3f& direction, float tnear, float tfar) noexcept
{
	const embree::Vec3f hitP = GetRayHitPosition(rayhit);

	SetRayPosition(rayhit, hitP);
	SetRayDirection(rayhit, direction);
	rayhit.ray.tnear = tnear;
	rayhit.ray.tfar = tfar;
	rayhit.ray.mask = -1;
	rayhit.ray.flags = 0;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
}

// Helper functions to set rays 8

__forceinline void SetRay8Position(RTCRayHit8* rayhit, const embree::Vec3f& position, const uint8_t index) noexcept
{
	rayhit->ray.org_x[index] = position.x;
	rayhit->ray.org_y[index] = position.y;
	rayhit->ray.org_z[index] = position.z;
}

__forceinline void SetRay8Direction(RTCRayHit8* rayhit, const embree::Vec3f& direction, const uint8_t index) noexcept
{
	rayhit->ray.dir_x[index] = direction.x;
	rayhit->ray.dir_y[index] = direction.y;
	rayhit->ray.dir_z[index] = direction.z;
}

__forceinline void SetRay8(RTCRayHit8* rayhit, const embree::Vec3f& position, const embree::Vec3f& direction, float t, const uint8_t index) noexcept
{
	SetRay8Position(rayhit, position, index);
	SetRay8Direction(rayhit, direction, index);
	rayhit->ray.tnear[index] = 0.0001f;
	rayhit->ray.tfar[index] = t;
	rayhit->ray.mask[index] = -1;
	rayhit->ray.flags[index] = 0;
	rayhit->hit.geomID[index] = RTC_INVALID_GEOMETRY_ID;
	rayhit->hit.instID[index][0] = RTC_INVALID_GEOMETRY_ID;
}

__forceinline embree::Vec3f GetRayHit8Position(RTCRayHit8* rayhit, const uint8_t index) noexcept
{
	return embree::Vec3f(rayhit->ray.org_x[index] + rayhit->ray.dir_x[index] * rayhit->ray.tfar[index],
		rayhit->ray.org_y[index] + rayhit->ray.dir_y[index] * rayhit->ray.tfar[index],
		rayhit->ray.org_z[index] + rayhit->ray.dir_z[index] * rayhit->ray.tfar[index]);
}

__forceinline embree::Vec3f GetRayHit8Normal(RTCRayHit8* rayhit, const uint8_t index) noexcept { return embree::Vec3f(rayhit->hit.Ng_x[index], rayhit->hit.Ng_y[index], rayhit->hit.Ng_z[index]); }

__forceinline void PostIntersect8Cleanup(RTCRayHit8* rayhit, const uint8_t index) noexcept
{
	const embree::Vec3f hitN = GetRayHit8Normal(rayhit, index);
	const embree::Vec3f hitP = GetRayHit8Position(rayhit, index);

	SetRay8(rayhit, hitP, hitN, 10000.0f, index);
}

__forceinline void PostIntersect8Prep(RTCRayHit8* rayhit, const embree::Vec3f& direction, float tnear, float tfar, const uint8_t index) noexcept
{
	const embree::Vec3f hitP = GetRayHit8Position(rayhit, index);

	SetRay8Position(rayhit, hitP, index);
	SetRay8Direction(rayhit, direction, index);
	rayhit->ray.tnear[index] = tnear;
	rayhit->ray.tfar[index] = tfar;
	rayhit->ray.mask[index] = -1;
	rayhit->ray.flags[index] = 0;
	rayhit->hit.geomID[index] = RTC_INVALID_GEOMETRY_ID;
	rayhit->hit.instID[0][index] = RTC_INVALID_GEOMETRY_ID;
}

__forceinline void SetRayNPosition(RTCRayHitNp rayhit, const embree::Vec3f& position, const uint8_t index) noexcept
{
	rayhit.ray.org_x[index] = position.x;
	rayhit.ray.org_y[index] = position.y;
	rayhit.ray.org_z[index] = position.z;
}

__forceinline void SetRayNDirection(RTCRayHitNp rayhit, const embree::Vec3f& direction, const uint8_t index) noexcept
{
	rayhit.ray.dir_x[index] = direction.x;
	rayhit.ray.dir_y[index] = direction.y;
	rayhit.ray.dir_z[index] = direction.z;
}

__forceinline void SetRayN(RTCRayHitNp rayhit, const embree::Vec3f& position, const embree::Vec3f& direction, float t, const uint8_t index) noexcept
{
	SetRayNPosition(rayhit, position, index);
	SetRayNDirection(rayhit, direction, index);
	rayhit.ray.tnear[index] = 0.0001f;
	rayhit.ray.tfar[index] = t;
	rayhit.ray.mask[index] = -1;
	rayhit.ray.flags[index] = 0;
	rayhit.hit.geomID[index] = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0][index] = RTC_INVALID_GEOMETRY_ID;
}

__forceinline embree::Vec3f GetRayHitNPosition(RTCRayHitNp rayhit, const uint8_t index) noexcept
{
	return embree::Vec3f(rayhit.ray.org_x[index] + rayhit.ray.dir_x[index] * rayhit.ray.tfar[index],
		rayhit.ray.org_y[index] + rayhit.ray.dir_y[index] * rayhit.ray.tfar[index],
		rayhit.ray.org_z[index] + rayhit.ray.dir_z[index] * rayhit.ray.tfar[index]);
}

__forceinline embree::Vec3f GetRayHitNNormal(RTCRayHitNp rayhit, const uint8_t index) noexcept { return embree::Vec3f(rayhit.hit.Ng_x[index], rayhit.hit.Ng_y[index], rayhit.hit.Ng_z[index]); }

__forceinline void PostIntersect8Cleanup(RTCRayHitNp rayhit, const uint8_t index) noexcept
{
	const embree::Vec3f hitN = GetRayHitNNormal(rayhit, index);
	const embree::Vec3f hitP = GetRayHitNPosition(rayhit, index);

	SetRayN(rayhit, hitP, hitN, 10000.0f, index);
}

__forceinline void PostIntersectNPrep(RTCRayHitNp rayhit, const embree::Vec3f& direction, float tnear, float tfar, const uint8_t index) noexcept
{
	const embree::Vec3f hitP = GetRayHitNPosition(rayhit, index);

	SetRayNPosition(rayhit, hitP, index);
	SetRayNDirection(rayhit, direction, index);
	rayhit.ray.tnear[index] = tnear;
	rayhit.ray.tfar[index] = tfar;
	rayhit.ray.mask[index] = -1;
	rayhit.ray.flags[index] = 0;
	rayhit.hit.geomID[index] = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0][index] = RTC_INVALID_GEOMETRY_ID;
}

