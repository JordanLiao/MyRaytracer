#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "Resources.h"
#include "GLM/glm.hpp"

class Triangle {
public:
	glm::vec3 positions[3]; //points A,B,C
	glm::vec3 normals[3]; //vertex normals
	glm::vec3 faceNormal; //normal of the triangle face; calculated from the vertex pos; different from the above vertex "normals".

	Triangle(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& n1, glm::vec3& n2, glm::vec3& n3);

	/*
		Check intersection of ray against THIS triangle. If ray does not intersect, return FLT_MAX.
		raySource: 3D position of the source of the supplied ray
		ray:: 3d direction of the ray
	*/
	float intersect(glm::vec3& baryCenter, const glm::vec3& raySource, const glm::vec3& ray);

private:
	//used for Barycentric Coordinates calculations
	float d00, d01, d11, denom;
	glm::vec3 v0, v1;
};

#endif
