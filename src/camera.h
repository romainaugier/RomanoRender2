#pragma once

#ifndef CAMERA
#define CAMERA

#include "mat44.h"
#include "maths.h"
#include "ray.h"

struct Camera
{
	vec3 origin;
	vec3 lower_left_corner;
	vec3 h, v;
	vec3 pos;
	vec3 posForRays;
	vec3 rotation;

	mat44 transformation_matrix;

	float focal_length;
	float fov;
	float aspect;
	float scale;
	
	Camera() {}

	Camera(vec3 _pos, vec3 _lookat, float focal, int xres, int yres) :
		pos(_pos),
		rotation(_lookat),
		focal_length(focal),
		aspect((float)xres / (float)yres)
	{
		transformation_matrix = mat44();
		vec3 u, v, w;
		vec3 up(0.0f, 1.0f, 0.0f);

		fov = 2 * rad2deg(std::atan(36.0f / (2 * focal_length)));
		scale = tan(deg2rad(fov * 0.5f));

		float theta = fov * PI / 180.0f;
		float half_height = tan(theta / 2.0f);
		float half_width = aspect * half_height;

		origin = pos;

		vec3 w_(pos - rotation);
		w = normalize(w_);
		u = normalize(cross(w, up));
		v = cross(w, u);

		lower_left_corner = origin - u * half_width - v * half_height - w;
		h = u * 2.0f * half_width;
		v = v * 2.0F * half_height;

	}

	RTCRayHit GetRay(float s, float t) const noexcept;

	void Update(int& xres, int& yres) noexcept;

	void SetTransform() noexcept;
	
	void SetTransformFromCam(const mat44& rotate_matrix) noexcept;

};

#endif // !CAMERA

