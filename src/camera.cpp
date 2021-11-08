#include "camera.h"

//
//RayHit Camera::GetRay(float s, float t) const noexcept
//{
//	RayHit rayhit;
//	rayhit.ray.origin = origin;
//	rayhit.ray.direction = lower_left_corner + h * s + v * t - origin;
//	
//	return rayhit;
//}

void Camera::Update(int& xres, int& yres) noexcept
{
	aspect = (float)xres / (float)yres;

	embree::Vec3f u, w;
	embree::Vec3f up(0.0f, 1.0f, 0.0f);

	fov = 2 * rad2deg(std::atan(36.0f / (2 * focal_length)));
	scale = tan(deg2rad(fov * 0.5f));

	float theta = fov * PI / 180.0f;
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

void Camera::SetTransform() noexcept
{
	mat44 translate_matrix = mat44();
	mat44 rotate_matrix = mat44();

	set_translation(translate_matrix, pos);
	set_rotation(rotate_matrix, rotation);

	transformation_matrix = translate_matrix * rotate_matrix;

	posForRays = embree::Vec3f(transformation_matrix[0][3], transformation_matrix[1][3], transformation_matrix[2][3]);
}

void Camera::SetTransformFromCam(const mat44& rotate_matrix) noexcept
{
	mat44 translate_matrix = mat44();

	set_translation(translate_matrix, pos);

	transformation_matrix = rotate_matrix * translate_matrix;
}