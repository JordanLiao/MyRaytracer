#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include <string>
#include <vector>
#include "GLM/glm.hpp"
#include "Resources.h"
#include "Object.h"

struct Intersect {
	glm::vec3 point;
	bool hasNormal;
	glm::vec3 normal;
	float distance;
	Resources::Material* mtl;
#ifdef _DEBUG
	std::string* name; //optional name of the corresponding obj name
#endif
};

enum class ObjectType {
	triangle,
	sphere,
	MDF,
};

namespace RayTracer {
	/*
		finds the nearest triangle to the camera that intersects with the ray; if there is no such a triangle return null
	*/
	bool trace(Intersect& intersect, const glm::vec3& raySource, const glm::vec3& ray, std::vector<Object*> objects);

	/*
		convert unit spherical coordinates theta and phi to directional vector on a hemisphere centered
		around the given normal.
	*/
	glm::vec3 toHemisphericalDirection(float theta, float phi, const glm::vec3& normal);

	glm::vec3 getBRDFImportanceSample(const glm::vec3& normal, const glm::vec3& reflection, float reflectiveness, float specularFocus);

	/*
		uniformly sample a direction on a hemisphere centered around a given normal
	*/
	glm::vec3 getHemisphericalSample(const glm::vec3& normal);

	glm::vec3 getSphericalSample();

	glm::vec3 getCosineWeightedSample(const glm::vec3& normal);

	float getUniformSample();
}

#endif