#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Core.h"

class Light {
public:
	glm::vec3 pos;
	glm::vec3 color;

	Light();
	Light(glm::vec3 position, glm::vec3 color);
};

#endif