#pragma once

#include "common/math/vec3.h"

#include "embree3/rtcore_ray.h"

#include <algorithm>
#include <stdint.h>

// Helper functions for rays
__forceinline void SetRayPosition(RTCRayHit* rayhit, const embree::Vec3f& position) noexcept { rayhit->ray.org_x = position.x; rayhit->ray.org_y = position.y; rayhit->ray.org_z = position.z; }
__forceinline void SetRayDirection(RTCRayHit* rayhit, const embree::Vec3f& direction) noexcept { rayhit->ray.dir_x = direction.x; rayhit->ray.dir_y = direction.y; rayhit->ray.dir_z = direction.z; }

__forceinline void SetRay(RTCRayHit* rayhit, const embree::Vec3f& position, const embree::Vec3f& direction, float t) noexcept
{
	SetRayPosition(rayhit, position);
	SetRayDirection(rayhit, direction);
	rayhit->ray.tnear = 0.0001f;
	rayhit->ray.tfar = t;
	rayhit->ray.mask = -1;
	rayhit->ray.flags = 0;
	rayhit->hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit->hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
}

__forceinline embree::Vec3f GetRayHitPosition(RTCRayHit* rayhit) noexcept
{
	return embree::Vec3f(rayhit->ray.org_x + rayhit->ray.dir_x * rayhit->ray.tfar,
		rayhit->ray.org_y + rayhit->ray.dir_y * rayhit->ray.tfar,
		rayhit->ray.org_z + rayhit->ray.dir_z * rayhit->ray.tfar);
}

__forceinline embree::Vec3f GetRayHitNormal(RTCRayHit* rayhit) noexcept { return embree::Vec3f(rayhit->hit.Ng_x, rayhit->hit.Ng_y, rayhit->hit.Ng_z); }

__forceinline void PostIntersectCleanup(RTCRayHit* rayhit) noexcept
{
	const embree::Vec3f hitN = GetRayHitNormal(rayhit);
	const embree::Vec3f hitP = GetRayHitPosition(rayhit);

	SetRay(rayhit, hitP, hitN, 10000.0f);
}

__forceinline void PostIntersectPrep(RTCRayHit* rayhit, const embree::Vec3f& direction, float tnear, float tfar) noexcept
{
	const embree::Vec3f hitP = GetRayHitPosition(rayhit);

	SetRayPosition(rayhit, hitP);
	SetRayDirection(rayhit, direction);
	rayhit->ray.tnear = tnear;
	rayhit->ray.tfar = tfar;
	rayhit->ray.mask = -1;
	rayhit->ray.flags = 0;
	rayhit->hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit->hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
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


// Helper functions for ray packets
struct HitGlobals
{
	float* p_x;
	float* p_y;
	float* p_z;

	float* n_x;
	float* n_y;
	float* n_z;

	float* s;
	float* t;

	embree::Vec3f* color;
};

struct RayPacket
{
	RTCRayHitNp rayHit;
	HitGlobals hitGlobals;

	embree::Vec3f* output;
};

// Allocate memory for a ray packet
__forceinline void AllocateRayPacketN(RayPacket& rayPacket, const int N) noexcept
{
	rayPacket.rayHit.ray.dir_x = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.ray.dir_y = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.ray.dir_z = (float*)embree::alignedMalloc(sizeof(float) * N, 16);

	rayPacket.rayHit.ray.org_x = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.ray.org_y = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.ray.org_z = (float*)embree::alignedMalloc(sizeof(float) * N, 16);

	rayPacket.rayHit.ray.flags = (unsigned int*)embree::alignedMalloc(sizeof(unsigned int) * N, 16);
	rayPacket.rayHit.ray.tnear = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.ray.tfar = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.ray.time = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.ray.mask = (unsigned int*)embree::alignedMalloc(sizeof(unsigned int) * N, 16);
	rayPacket.rayHit.ray.id = (unsigned int*)embree::alignedMalloc(sizeof(unsigned int) * N, 16);

	rayPacket.rayHit.hit.Ng_x = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.hit.Ng_y = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.hit.Ng_z = (float*)embree::alignedMalloc(sizeof(float) * N, 16);

	rayPacket.rayHit.hit.u = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.rayHit.hit.v = (float*)embree::alignedMalloc(sizeof(float) * N, 16);

	rayPacket.rayHit.hit.primID = (unsigned int*)embree::alignedMalloc(sizeof(unsigned int) * N, 16);
	rayPacket.rayHit.hit.geomID = (unsigned int*)embree::alignedMalloc(sizeof(unsigned int) * N, 16);
	rayPacket.rayHit.hit.instID[0] = (unsigned int*)embree::alignedMalloc(sizeof(unsigned int) * N, 16);

	rayPacket.hitGlobals.p_x = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.hitGlobals.p_y = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.hitGlobals.p_z = (float*)embree::alignedMalloc(sizeof(float) * N, 16);

	rayPacket.hitGlobals.n_x = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.hitGlobals.n_y = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.hitGlobals.n_z = (float*)embree::alignedMalloc(sizeof(float) * N, 16);

	rayPacket.hitGlobals.s = (float*)embree::alignedMalloc(sizeof(float) * N, 16);
	rayPacket.hitGlobals.t = (float*)embree::alignedMalloc(sizeof(float) * N, 16);

	rayPacket.hitGlobals.color = (embree::Vec3f*)embree::alignedMalloc(sizeof(embree::Vec3f) * N, 16);
}

// Free memory of an allocated ray packet
__forceinline void FreeRayPacketN(RayPacket& rayPacket) noexcept
{
	embree::alignedFree(rayPacket.rayHit.ray.dir_x);
	embree::alignedFree(rayPacket.rayHit.ray.dir_y);
	embree::alignedFree(rayPacket.rayHit.ray.dir_z);

	embree::alignedFree(rayPacket.rayHit.ray.org_x);
	embree::alignedFree(rayPacket.rayHit.ray.org_y);
	embree::alignedFree(rayPacket.rayHit.ray.org_z);

	embree::alignedFree(rayPacket.rayHit.ray.flags);
	embree::alignedFree(rayPacket.rayHit.ray.tnear);
	embree::alignedFree(rayPacket.rayHit.ray.tfar);
	embree::alignedFree(rayPacket.rayHit.ray.mask);
	embree::alignedFree(rayPacket.rayHit.ray.time);
	embree::alignedFree(rayPacket.rayHit.ray.id);

	embree::alignedFree(rayPacket.rayHit.hit.Ng_x);
	embree::alignedFree(rayPacket.rayHit.hit.Ng_y);
	embree::alignedFree(rayPacket.rayHit.hit.Ng_z);

	embree::alignedFree(rayPacket.rayHit.hit.u);
	embree::alignedFree(rayPacket.rayHit.hit.v);

	embree::alignedFree(rayPacket.rayHit.hit.primID);
	embree::alignedFree(rayPacket.rayHit.hit.geomID);
	embree::alignedFree(rayPacket.rayHit.hit.instID[0]);

	embree::alignedFree(rayPacket.hitGlobals.p_x);
	embree::alignedFree(rayPacket.hitGlobals.p_y);
	embree::alignedFree(rayPacket.hitGlobals.p_z);
	
	embree::alignedFree(rayPacket.hitGlobals.n_x);
	embree::alignedFree(rayPacket.hitGlobals.n_y);
	embree::alignedFree(rayPacket.hitGlobals.n_z);

	embree::alignedFree(rayPacket.hitGlobals.s);
	embree::alignedFree(rayPacket.hitGlobals.t);

	embree::alignedFree(rayPacket.hitGlobals.color);
}

__forceinline void SortRayPacketGeomIDArray(RTCRayHitNp& rayhit, uint16_t N) noexcept
{
	std::sort(&rayhit.hit.geomID[0], &rayhit.hit.geomID[N], [](const int a, const int b) { return a > b; });
}

// Sort the different arrays of the ray packets, and return the index where missed intersection starts
__forceinline uint32_t SortRayPacketNByIntersectState(RTCRayHitNp& rayhit, uint16_t N) noexcept
{
	// Sort the geomID array of the ray packet
	SortRayPacketGeomIDArray(rayhit, N);
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

