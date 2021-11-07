//#include "boundingbox.h"
//
//bool Slabs(const BoundingBox& bbox, const RayHit& rayhit) noexcept
//{
//	float t_min = 0.0f;
//	float t_max = std::numeric_limits<float>::infinity();
//
//	for (int i = 0; i < 3; i++)
//	{
//		const float invD = rayhit.ray.inverseDirection[i];
//		float t0 = (bbox.p0[i] - rayhit.ray.origin[i]) * invD;
//		float t1 = (bbox.p1[i] - rayhit.ray.origin[i]) * invD;
//
//		if (invD < 0.0f) std::swap(t0, t1);
//
//		t_min = t0 > t_min ? t0 : t_min;
//		t_max = t1 < t_max ? t1 : t_max;
//
//		if (t_max <= t_min) return false;
//	}
//
//	return true;
//}
//
//
//bool* Slabs8(const BoundingBox* bbox, const RayHit& rayhit) noexcept
//{
//	bool hits[8] = { true };
//
//	for (int j = 0; j < 8; j++)
//	{
//		float t_min = 0.0f;
//		float t_max = std::numeric_limits<float>::infinity();
//
//		for (int i = 0; i < 3; i++)
//		{
//			const float invD = rayhit.ray.inverseDirection[i];
//			float t0 = (bbox[i].p0[i] - rayhit.ray.origin[i]) * invD;
//			float t1 = (bbox[i].p1[i] - rayhit.ray.origin[i]) * invD;
//
//			if (invD < 0.0f) std::swap(t0, t1);
//
//			t_min = t0 > t_min ? t0 : t_min;
//			t_max = t1 < t_max ? t1 : t_max;
//
//			if (t_max <= t_min) hits[j] = false;
//		}
//	}
//	
//	return hits;
//}
//
//bool SlabsOcclude(const BoundingBox& bbox, const ShadowRay& shadow) noexcept
//{
//	float t_min = 0.0f;
//	float t_max = std::numeric_limits<float>::infinity();
//
//	for (int i = 0; i < 3; i++)
//	{
//		const float invD = shadow.inverseDirection[i];
//		float t0 = (bbox.p0[i] - shadow.origin[i]) * invD;
//		float t1 = (bbox.p1[i] - shadow.origin[i]) * invD;
//
//		if (invD < 0.0f) std::swap(t0, t1);
//
//		t_min = t0 > t_min ? t0 : t_min;
//		t_max = t1 < t_max ? t1 : t_max;
//
//		if (t_max <= t_min) return false;
//	}
//
//	return true;
//}
//
//BoundingBox Union(const BoundingBox& x, const BoundingBox& y) noexcept
//{
//	const vec3 p0 = min(x.p0, y.p0);
//	const vec3 p1 = max(x.p1, y.p1);
//
//	return BoundingBox(p0, p1);
//}
//
//vec3 Offset(const BoundingBox& bbox, const vec3& p) noexcept
//{
//	vec3 o = p - bbox.p0;
//	if (bbox.p1.x > bbox.p0.x) o.x /= (bbox.p1.x - bbox.p0.x);
//	if (bbox.p1.y > bbox.p0.y) o.y /= (bbox.p1.y - bbox.p0.y);
//	if (bbox.p1.z > bbox.p0.z) o.z /= (bbox.p1.z - bbox.p0.z);
//	return o;
//}
//
//float SurfaceArea(const BoundingBox& b) noexcept
//{
//	const vec3 d = b.p1 - b.p0;
//	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
//}
//
//uint8_t MaximumDimension(const BoundingBox& a) noexcept
//{
//	const vec3 diag = a.p1 - a.p0;
//	if (diag.x > diag.y && diag.x > diag.z) return 0;
//	else if (diag.y > diag.z) return 1;
//	else return 2;
//}