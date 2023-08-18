#pragma once
#include <glm/glm.hpp>

//using namespace glm;


namespace Core
{
	namespace Math
	{
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
			void Create(glm::vec3& p, glm::vec3& q, glm::vec3& r);
			bool Intersect(const glm::vec3& bbmin, const glm::vec3& bbmax);// bounding box
			bool Intersect(const glm::vec3& center, float beamRadius);// with sphere
			POINT_STATE ClassifyPoint(const glm::vec3& point, float& dist);
			float GetDistance(const glm::vec3& point);
		};
	}
}
