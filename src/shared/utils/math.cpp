#include "math.hpp"

constexpr float PI{ 3.14159265f };
constexpr float M_RADPI{ 57.2957795f };

float math::degToRad(const float deg)
{
	return deg * (PI / 180.0f);
}

float math::radToDeg(const float rad)
{
	return rad * (180.0f / PI);
}

inline float math::normalizeAngle(float ang)
{
	float f = (ang * (1.f / 360.f)) + 0.5f;
	int i = (int)f;
	float fi = (float)i;
	f = (f < 0.f && f != fi) ? fi - 1.f : fi;
	ang -= f * 360.f;
	return ang;
}

void math::sinCos(const float radians, float *const sine, float *const cosine)
{
	*sine = std::sinf(radians);
	*cosine = std::cosf(radians);
}

void math::clampAngles(vec3 &v)
{
	v.x = std::clamp(normalizeAngle(v.x), -89.f, 89.f);
	v.y = normalizeAngle(v.y);
	v.z = 0.0f;
}

float math::lerp(const float a, const float b, const float t)
{
	return a + (b - a) * t;
}

void math::vectorAngles(const vec3 &forward, vec3 &angles)
{
	float yaw{};
	float pitch{};

	if (forward.y == 0.0f && forward.x == 0.0f)
	{
		yaw = 0.0f;

		if (forward.z > 0.0f) {
			pitch = 270.0f;
		}

		else {
			pitch = 90.0f;
		}
	}

	else
	{
		yaw = radToDeg(std::atan2f(forward.y, forward.x));

		if (yaw < 0.0f) {
			yaw += 360.0f;
		}

		pitch = radToDeg(std::atan2f(-forward.z, forward.length2D()));

		if (pitch < 0.0f) {
			pitch += 360.0f;
		}
	}

	angles.set(pitch, yaw, 0.0f);
}

void math::angleVectors(const vec3 &angles, vec3 *forward, vec3 *right, vec3 *up)
{
	float sr{};
	float sp{};
	float sy{};
	float cr{};
	float cp{};
	float cy{};

	sinCos(degToRad(angles.x), &sp, &cp);
	sinCos(degToRad(angles.y), &sy, &cy);
	sinCos(degToRad(angles.z), &sr, &cr);

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = -1.0f * sr * sp * cy + -1.0f * cr * -sy;
		right->y = -1.0f * sr * sp * sy + -1.0f * cr * cy;
		right->z = -1.0f * sr * cp;
	}

	if (up) {
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

vec3 math::calcAngle(const vec3 &from, const vec3 &to, bool clamp)
{
	const vec3 delta{ from - to };
	const float hyp{ std::sqrtf((delta.x * delta.x) + (delta.y * delta.y)) };

	vec3 out{ (std::atanf(delta.z / hyp) * M_RADPI), (std::atanf(delta.y / delta.x) * M_RADPI), 0.0f };

	if (delta.x >= 0.0f) {
		out.y += 180.0f;
	}

	if (clamp) {
		clampAngles(out);
	}

	return out;
}

float math::calcFOV(const vec3 &ang0, const vec3 &ang1)
{
	vec3 v_ang0{};
	angleVectors(ang0, &v_ang0);

	vec3 v_ang1{};
	angleVectors(ang1, &v_ang1);

	const float out{ radToDeg(acos(v_ang1.dot(v_ang0) / v_ang1.lengthSqr())) };

	if (!isfinite(out) || isinf(out) || isnan(out)) {
		return 0.0f;
	}

	return out;
}

float math::remap(const float val, const float in_min, const float in_max, const float out_min, const float out_max)
{
	if (in_min == in_max) {
		return val >= in_max ? out_max : out_min;
	}

	return out_min + (out_max - out_min) * std::clamp((val - in_min) / (in_max - in_min), 0.0f, 1.0f);
}

void math::vectorTransform(const vec3 &input, const matrix3x4 &matrix, vec3 &output)
{
	for (int n{}; n < 3; n++) {
		output[n] = input.dot((vec3 &)matrix[n]) + matrix[n][3];
	}
}

bool math::rayVsBox(const vec3 &ray_origin, const vec3 &ray_angles, const vec3 &box_origin, const vec3 &box_mins, const vec3 &box_maxs)
{
	vec3 ray_dir{};

	angleVectors(ray_angles, &ray_dir);

	const vec3 box_min_world{ box_origin + box_mins };
	const vec3 box_max_world{ box_origin + box_maxs };

	float tmin{ (box_min_world.x - ray_origin.x) / ray_dir.x };
	float tmax{ (box_max_world.x - ray_origin.x) / ray_dir.x };

	if (tmin > tmax) {
		std::swap(tmin, tmax);
	}

	float tymin{ (box_min_world.y - ray_origin.y) / ray_dir.y };
	float tymax{ (box_max_world.y - ray_origin.y) / ray_dir.y };

	if (tymin > tymax) {
		std::swap(tymin, tymax);
	}

	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}

	if (tymin > tmin) {
		tmin = tymin;
	}

	if (tymax < tmax) {
		tmax = tymax;
	}

	float tzmin{ (box_min_world.z - ray_origin.z) / ray_dir.z };
	float tzmax{ (box_max_world.z - ray_origin.z) / ray_dir.z };

	if (tzmin > tzmax) {
		std::swap(tzmin, tzmax);
	}

	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}

	if (tzmin > tmin) {
		tmin = tzmin;
	}

	if (tzmax < tmax) {
		tmax = tzmax;
	}

	return tmax >= 0.0f && tmin <= tmax;
}

void math::matrixGetColumn(const matrix3x4 &matrix, const int column, vec3 &out)
{
	out.x = matrix[0][column];
	out.y = matrix[1][column];
	out.z = matrix[2][column];
}
