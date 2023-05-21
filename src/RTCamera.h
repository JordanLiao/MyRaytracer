#ifndef _RTCAMERA_H_
#define _RTCAMERA_H_

#include "Core.h"

class RTCamera {
public:
	glm::vec3 pos;
	glm::vec3 lookAt;
	glm::vec3 lookDir;
	glm::vec3 up;
	glm::vec3 camLeft;
	glm::vec3 camRight;
	glm::vec3 camUp;
	glm::vec3 camDown;
	glm::mat4 view;
	glm::mat4 viewInverse;
	glm::vec4 worldOriginInverse;
	float aspectRatio;
	float vfov; //vertical fov in radian
	float tangent; //tan(0.5 * vfov)
	unsigned int width, height; //resolution size of the camera 

	RTCamera();
	RTCamera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up, float aspectRatio, float vfov, unsigned int width, unsigned int height);

	/*
		generate a ray that shoots through a given pixel (x, y)'s center
	*/
	glm::vec3 generateCenterAlignedRay(unsigned int x, unsigned int y);

	/*
		generate a ray that shoots randomly through a given pixel (x, y)'s field of view
	*/
	glm::vec3 generateRay(float x, float y);

	void translate(const glm::vec3& offset);
	void rotate(const glm::vec3& axis, const float& rad);
	
private:
	void updateView();
};

#endif
