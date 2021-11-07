//#include "sphere.h"
//
//bool SphereHit(const Sphere& sphere, RayHit& rayhit) noexcept
//{
//	const vec3 oc = rayhit.ray.origin - sphere.center;
//	const auto a = length2(rayhit.ray.direction);
//	const auto b = dot(oc, rayhit.ray.direction);
//	const auto c = length2(oc) - sphere.radius * sphere.radius;
//	const auto discriminant = b * b - a * c;
//	
//	if (discriminant > 0)
//	{
//		const float t = (-b - sqrtf(discriminant)) / a;
//
//		if (t > 0.0f && t < rayhit.ray.t)
//		{
//			rayhit.ray.t = t;
//			rayhit.hit.pos = rayhit.ray.origin + (rayhit.ray.direction * t);
//			rayhit.hit.normal = (rayhit.hit.pos - sphere.center) / sphere.radius;
//			rayhit.hit.geomID = sphere.id;
//			rayhit.hit.matID = sphere.matId;
//
//			return true;
//		}
//	}
//	
//	return false;
//}
//
//bool SphereHitN(const Sphere* spheres, RayHit& rayhit, const int count) noexcept
//{
//	bool hit = false;
//
//	//for (int i = 0; i < count; i++)
//	//{
//	//	if (SphereHit(spheres[i], rayhit))
//	//	{
//	//		hit = true;
//	//	}
//	//}
//
//	float centersX[8];
//	float centersY[8];
//	float centersZ[8];
//	float radius[8];
//	float t[8];
//
//	for (int i = 0; i < count; i++)
//	{
//		centersX[i] = spheres[i].center.x;
//		centersY[i] = spheres[i].center.y;
//		centersZ[i] = spheres[i].center.z;
//		radius[i] = spheres[i].radius;
//		t[i] = rayhit.ray.t;
//	}
//
//	ispc::SphereHitN(centersX, centersY, centersZ, radius, rayhit.ray.origin, rayhit.ray.direction, t, count);
//
//	int8_t idx = -1;
//
//	float tmpT = std::min(rayhit.ray.t, 10000000.0f);
//
//	for (int i = 0; i < count; i++)
//	{
//		if (t[i] < tmpT && t[i] > 0.001f)
//		{
//			tmpT = t[i];
//			idx = i;
//		}
//	}
//
//	if (idx >= 0)
//	{
//		hit = true;
//		rayhit.ray.t = tmpT;
//		rayhit.hit.pos = rayhit.ray.origin + (rayhit.ray.direction * tmpT);
//		rayhit.hit.normal = (rayhit.hit.pos - spheres[idx].center) / spheres[idx].radius;
//		rayhit.hit.geomID = spheres[idx].id;
//		rayhit.hit.matID = spheres[idx].matId;
//	}
//
//	return hit;
//}
//
//bool SphereOcclude(const Sphere& sphere, ShadowRay& shadow) noexcept
//{
//	const vec3 oc = shadow.origin - sphere.center;
//	const auto a = length2(shadow.direction);
//	const auto b = dot(oc, shadow.direction);
//	const auto c = length2(oc) - sphere.radius * sphere.radius;
//	const auto discriminant = b * b - a * c;
//	
//	if (discriminant > 0)
//	{
//		const float t = (-b - sqrtf(discriminant)) / a;
//
//		if (t > 0.001f)
//		{
//			shadow.t = t;
//			return true;
//		}
//	}
//
//	return false;
//}
//
//bool SphereOccludeN(const Sphere* spheres, ShadowRay& shadow, const int count) noexcept
//{
//	bool hit = false;
//
//	for (int i = 0; i < count; i++)
//	{
//		const Sphere sphere = spheres[i];
//
//		if (SphereOcclude(sphere, shadow))
//		{
//			hit = true;
//			break;
//		}
//	}
//
//	//vec3 centers[8];
//	//float radius[8];
//	//float t[8];
//
//	//for (int i = 0; i < count; i++)
//	//{
//	//	centers[i] = spheres[i].center;
//	//	radius[i] = spheres[i].radius;
//	//	t[i] = shadow.t;
//	//}
//
//
//	//ispc::SphereHitN(centers, radius, shadow.origin, shadow.direction, t, count);
//
//	//float tmpT = std::min(shadow.t, 10000000.0f);
//
//	//for (int i = 0; i < count; i++)
//	//{
//	//	if (t[i] < tmpT && t[i] > 0.001f)
//	//	{
//	//		tmpT = t[i];
//	//		hit = true;
//	//		break;
//	//	}
//	//}
//
//	return hit;
//}
//
//BoundingBox SphereBBox(const Sphere& sphere) noexcept
//{
//	return BoundingBox(vec3(sphere.center - sphere.radius), vec3(sphere.center + sphere.radius));
//}