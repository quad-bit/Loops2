#pragma once
#include <glm\glm.hpp>

using namespace glm;

enum POINT_STATE
{
	PLANE_FRONT,
	PLANE_BACK,
	ON_PLANE,
	NONE
};

class Plane
{
private:
	float a, b, c, d; // ax + by + cz + d = 0

public:
	Plane() : a(0), b(0), c(0), d(0) {}
	Plane(float A, float B, float C, float D) : a(A), b(B), c(C), d(D) {}
	void Create(vec3& p, vec3& q, vec3& r);
	bool Intersect(const vec3& bbmin, const vec3& bbmax);// bounding box
	bool Intersect(const vec3& center, float beamRadius);// with sphere
	POINT_STATE ClassifyPoint(const vec3& point, float& dist);
	float GetDistance(const vec3& point);
};
