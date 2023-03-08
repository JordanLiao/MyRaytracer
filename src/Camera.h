#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Core.h"

class Camera {
public:
	glm::vec3 pos;
	glm::vec3 look;
	glm::vec3 up;
	glm::mat4 view;
	glm::mat4 viewInverse;
	glm::vec4 worldOriginInverse;
	float aspectRatio;
	float vfov; //vertical fov in radian
	float tangent; //tan(0.5 * vfov)
	unsigned int width, height; //resolution size of the camera 

	Camera();
	Camera(glm::vec3 pos, glm::vec3 look, float aspectRatio, float vfov, unsigned int width, unsigned int height, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f));

	void generateRay(unsigned int x, unsigned int y, glm::vec3& ray);
private:
};

#endif
