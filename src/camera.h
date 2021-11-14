#pragma once

#include "mat44.h"
#include "ray.h"
#include "ispc/ray_generation.h"

struct Camera
{
	embree::Vec3f origin;
	embree::Vec3f lower_left_corner;
	embree::Vec3f h, v;
	embree::Vec3f pos;
	embree::Vec3f posForRays;
	embree::Vec3f rotation;

	mat44 transformation_matrix;
	ispc::mat44 ispcTransformMatrix;

	float focal_length;
	float fov;
	float aspect;
	float scale;
	
	Camera() {}

	Camera(embree::Vec3f _pos, embree::Vec3f _lookat, float focal, int xres, int yres) :
		pos(_pos),
		rotation(_lookat),
		focal_length(focal),
		aspect((float)xres / (float)yres)
	{
		transformation_matrix = mat44();
		embree::Vec3f u, v, w;
		embree::Vec3f up(0.0f, 1.0f, 0.0f);

		fov = 2 * embree::rad2deg(embree::atan(36.0f / (2 * focal_length)));
		scale = embree::tan(embree::deg2rad(fov * 0.5f));

		float theta = fov * float(embree::pi) / 180.0f;
		float half_height = embree::tan(theta / 2.0f);
		float half_width = aspect * half_height;

		origin = pos;

		embree::Vec3f w_(pos - rotation);
		w = embree::normalize(w_);
		u = embree::normalize(embree::cross(w, up));
		v = embree::cross(w, u);

		lower_left_corner = origin - u * half_width - v * half_height - w;
		h = u * 2.0f * half_width;
		v = v * 2.0F * half_height;

	}

	RTCRayHit GetRay(float s, float t) const noexcept;

	void Update(int& xres, int& yres) noexcept;

	void SetTransform() noexcept;
	
	void SetTransformFromCam(const mat44& rotate_matrix) noexcept;

};
