#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <string>
#include <vector>
#include "Resources.h"
#include "GLM/glm.hpp"

class Sphere {
public:
	Resources::Material* material;

	glm::vec3 position;
	float radius;

	Sphere();
	Sphere(const glm::vec3 & position, float radius, Resources::Material* mtl);
	~Sphere();

	float intersect(const glm::vec3& raySource, const glm::vec3& ray);

private:
	bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1);

};

#endif