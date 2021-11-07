#pragma once

#ifndef RAY
#define RAY

#include "maths.h"

#include "embree3/rtcore_ray.h"

#include <limits>
#include <stdint.h>
#include <immintrin.h>

// Helper functions to set rays

__forceinline void SetRayPosition(RTCRayHit* rayhit, const vec3& position) noexcept { rayhit->ray.org_x = position.x; rayhit->ray.org_y = position.y; rayhit->ray.org_z = position.z; }
__forceinline void SetRayDirection(RTCRayHit* rayhit, const vec3& direction) noexcept { rayhit->ray.dir_x = direction.x; rayhit->ray.dir_y = direction.y; rayhit->ray.dir_z = direction.z; }

__forceinline void SetRay(RTCRayHit* rayhit, const vec3& position, const vec3& direction, float t) noexcept
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

__forceinline vec3 GetRayHitPosition(RTCRayHit* rayhit) noexcept 
{	
	return vec3(rayhit->ray.org_x + rayhit->ray.dir_x * rayhit->ray.tfar, 
				rayhit->ray.org_y + rayhit->ray.dir_y * rayhit->ray.tfar,
				rayhit->ray.org_z + rayhit->ray.dir_z * rayhit->ray.tfar); 
}

__forceinline vec3 GetRayHitNormal(RTCRayHit* rayhit) noexcept { return vec3(rayhit->hit.Ng_x, rayhit->hit.Ng_y, rayhit->hit.Ng_z); }

__forceinline void PostIntersectCleanup(RTCRayHit* rayhit) noexcept
{
	const vec3 hitN = GetRayHitNormal(rayhit);
	const vec3 hitP = GetRayHitPosition(rayhit);

	SetRay(rayhit, hitP, hitN, 10000.0f);
}

__forceinline void PostIntersectPrep(RTCRayHit* rayhit, const vec3& direction, float tnear, float tfar) noexcept
{
	const vec3 hitP = GetRayHitPosition(rayhit);

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

__forceinline void SetRay8Position(RTCRayHit8* rayhit, const vec3& position, const uint8_t index) noexcept 
{	
	rayhit->ray.org_x[index] = position.x; 
	rayhit->ray.org_y[index] = position.y; 
	rayhit->ray.org_z[index] = position.z; 
}

__forceinline void SetRay8Direction(RTCRayHit8* rayhit, const vec3& direction, const uint8_t index) noexcept 
{ 
	rayhit->ray.dir_x[index] = direction.x; 
	rayhit->ray.dir_y[index] = direction.y; 
	rayhit->ray.dir_z[index] = direction.z; 
}

__forceinline void SetRay8(RTCRayHit8* rayhit, const vec3& position, const vec3& direction, float t, const uint8_t index) noexcept
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

__forceinline vec3 GetRayHit8Position(RTCRayHit8* rayhit, const uint8_t index) noexcept
{
	return vec3(rayhit->ray.org_x[index] + rayhit->ray.dir_x[index] * rayhit->ray.tfar[index],
		rayhit->ray.org_y[index] + rayhit->ray.dir_y[index] * rayhit->ray.tfar[index],
		rayhit->ray.org_z[index] + rayhit->ray.dir_z[index] * rayhit->ray.tfar[index]);
}

__forceinline vec3 GetRayHit8Normal(RTCRayHit8* rayhit, const uint8_t index) noexcept { return vec3(rayhit->hit.Ng_x[index], rayhit->hit.Ng_y[index], rayhit->hit.Ng_z[index]); }

__forceinline void PostIntersect8Cleanup(RTCRayHit8* rayhit, const uint8_t index) noexcept
{
	const vec3 hitN = GetRayHit8Normal(rayhit, index);
	const vec3 hitP = GetRayHit8Position(rayhit, index);

	SetRay8(rayhit, hitP, hitN, 10000.0f, index);
}

__forceinline void PostIntersect8Prep(RTCRayHit8* rayhit, const vec3& direction, float tnear, float tfar, const uint8_t index) noexcept
{
	const vec3 hitP = GetRayHit8Position(rayhit, index);

	SetRay8Position(rayhit, hitP, index);
	SetRay8Direction(rayhit, direction, index);
	rayhit->ray.tnear[index] = tnear;
	rayhit->ray.tfar[index] = tfar;
	rayhit->ray.mask[index] = -1;
	rayhit->ray.flags[index] = 0;
	rayhit->hit.geomID[index] = RTC_INVALID_GEOMETRY_ID;
	rayhit->hit.instID[0][index] = RTC_INVALID_GEOMETRY_ID;
}

// Helper functions to set ray N in soa 

using RTCRayHit256 = RTCRayHitNt<256>;

__forceinline void AllocateRayhitNp(RTCRayHitNp& tmp, const int N)
{
	tmp.ray.dir_x = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.ray.dir_y = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.ray.dir_z = (float*)_aligned_malloc(sizeof(float) * N, 16);

	tmp.ray.org_x = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.ray.org_y = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.ray.org_z = (float*)_aligned_malloc(sizeof(float) * N, 16);

	tmp.ray.flags = (unsigned int*)_aligned_malloc(sizeof(unsigned int) * N, 16);
	tmp.ray.tnear = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.ray.tfar = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.ray.time = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.ray.mask = (unsigned int*)_aligned_malloc(sizeof(unsigned int) * N, 16);
	tmp.ray.id = (unsigned int*)_aligned_malloc(sizeof(unsigned int) * N, 16);

	tmp.hit.Ng_x = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.hit.Ng_y = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.hit.Ng_z = (float*)_aligned_malloc(sizeof(float) * N, 16);

	tmp.hit.u = (float*)_aligned_malloc(sizeof(float) * N, 16);
	tmp.hit.v = (float*)_aligned_malloc(sizeof(float) * N, 16);

	tmp.hit.primID = (unsigned int*)_aligned_malloc(sizeof(unsigned int) * N, 16);
	tmp.hit.geomID = (unsigned int*)_aligned_malloc(sizeof(unsigned int) * N, 16);
	tmp.hit.instID[0] = (unsigned int*)_aligned_malloc(sizeof(unsigned int) * N, 16);
}

__forceinline void FreeRayhitNp(RTCRayHitNp& tmp)
{
	_aligned_free(tmp.ray.dir_x);
	_aligned_free(tmp.ray.dir_y);
	_aligned_free(tmp.ray.dir_z);

	_aligned_free(tmp.ray.org_x);
	_aligned_free(tmp.ray.org_y);
	_aligned_free(tmp.ray.org_z);

	_aligned_free(tmp.ray.flags);
	_aligned_free(tmp.ray.tnear);
	_aligned_free(tmp.ray.tfar);
	_aligned_free(tmp.ray.mask);
	_aligned_free(tmp.ray.time);
	_aligned_free(tmp.ray.id);

	_aligned_free(tmp.hit.Ng_x);
	_aligned_free(tmp.hit.Ng_y);
	_aligned_free(tmp.hit.Ng_z);

	_aligned_free(tmp.hit.u);
	_aligned_free(tmp.hit.v);

	_aligned_free(tmp.hit.primID);
	_aligned_free(tmp.hit.geomID);
	_aligned_free(tmp.hit.instID[0]);
}

__forceinline void SetRayNPosition(RTCRayHitNp rayhit, const vec3& position, const uint8_t index) noexcept
{
	rayhit.ray.org_x[index] = position.x;
	rayhit.ray.org_y[index] = position.y;
	rayhit.ray.org_z[index] = position.z;
}

__forceinline void SetRayNDirection(RTCRayHitNp rayhit, const vec3& direction, const uint8_t index) noexcept
{
	rayhit.ray.dir_x[index] = direction.x;
	rayhit.ray.dir_y[index] = direction.y;
	rayhit.ray.dir_z[index] = direction.z;
}

__forceinline void SetRayN(RTCRayHitNp rayhit, const vec3& position, const vec3& direction, float t, const uint8_t index) noexcept
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

__forceinline vec3 GetRayHitNPosition(RTCRayHitNp rayhit, const uint8_t index) noexcept
{
	return vec3(rayhit.ray.org_x[index] + rayhit.ray.dir_x[index] * rayhit.ray.tfar[index],
		rayhit.ray.org_y[index] + rayhit.ray.dir_y[index] * rayhit.ray.tfar[index],
		rayhit.ray.org_z[index] + rayhit.ray.dir_z[index] * rayhit.ray.tfar[index]);
}

__forceinline vec3 GetRayHitNNormal(RTCRayHitNp rayhit, const uint8_t index) noexcept { return vec3(rayhit.hit.Ng_x[index], rayhit.hit.Ng_y[index], rayhit.hit.Ng_z[index]); }

__forceinline void PostIntersect8Cleanup(RTCRayHitNp rayhit, const uint8_t index) noexcept
{
	const vec3 hitN = GetRayHitNNormal(rayhit, index);
	const vec3 hitP = GetRayHitNPosition(rayhit, index);

	SetRayN(rayhit, hitP, hitN, 10000.0f, index);
}

__forceinline void PostIntersectNPrep(RTCRayHitNp rayhit, const vec3& direction, float tnear, float tfar, const uint8_t index) noexcept
{
	const vec3 hitP = GetRayHitNPosition(rayhit, index);

	SetRayNPosition(rayhit, hitP, index);
	SetRayNDirection(rayhit, direction, index);
	rayhit.ray.tnear[index] = tnear;
	rayhit.ray.tfar[index] = tfar;
	rayhit.ray.mask[index] = -1;
	rayhit.ray.flags[index] = 0;
	rayhit.hit.geomID[index] = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0][index] = RTC_INVALID_GEOMETRY_ID;
}


#endif // !RAY

