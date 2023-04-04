#ifndef _QUADLIGHT_H_
#define _QUADLIGHT_H_

#include <string>

#include "GLM/glm.hpp"
#include "Object.h"

class QuadLight {
public:
	glm::vec3 a, b, c, d; //vertex positions
	glm::vec3 radiance;
	glm::vec3 normal;
	glm::vec3 ab, cb; //side vector ab and bc
	float area;
	Object * obj;

	QuadLight();
	/*
		use a, b, c to figure out d. b sits in the corner of angle <abc
		  a-d 
		 / /
		b-c
	*/
	QuadLight(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 radiance);

private:
};

#endif