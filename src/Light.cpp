#include "Light.h"

Light::Light() {
	pos = glm::vec3(0.f, 0.f, 0.f);
	color = glm::vec3(0.9f, 0.9f, 0.9f);
}

Light::Light(glm::vec3 position, glm::vec3 color) {
	this->pos = position;
	this->color = color;
}
