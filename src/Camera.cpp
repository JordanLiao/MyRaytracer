#include "Camera.h"

Camera::Camera() {
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	lookAt = glm::vec3(0.0f, 0.0f, -1.f);
	up = glm::vec3(0.f, 1.f, 0.f);
	view = glm::lookAt(pos, lookAt, up);
	viewInverse = glm::inverse(view);
	worldOriginInverse = viewInverse * glm::vec4(0.f, 0.f, 0.f, 1.0f);
	aspectRatio = 16.f / 9.f;
	vfov = glm::radians(60.f);
	tangent = glm::tan(vfov * 0.5f);
	width = 320;
	height = 180;
}

Camera::Camera(glm::vec3 pos, glm::vec3 lookAt, float aspectRatio, float vfov, unsigned int width, unsigned int height, glm::vec3 up) {
	this->pos = pos;
	this->lookAt = lookAt;
	this->up = up;
	this->aspectRatio = aspectRatio;
	view = glm::lookAt(pos, lookAt, up);
	viewInverse = glm::inverse(view);
	worldOriginInverse = viewInverse * glm::vec4(0.f, 0.f, 0.f, 1.0f);
	this->vfov = vfov;
	tangent = glm::tan(vfov * 0.5f);
	this->width = width;
	this->height = height;
}

glm::vec3 Camera::generateCenterAlignedRay(unsigned int x, unsigned int y) {
	//float x and y in NDC, -1 to 1; y value is flipped since screen space is upside down
	float xf = (2.f * ((float)x + 0.5f) / (float)width - 1.f) * tangent * aspectRatio; //plus 0.5 to shoot through the center of a pixel
	float yf = (1.f - 2.f * ((float)y + 0.5f) / (float)height) * tangent;
	glm::vec4 ray0 = viewInverse * glm::vec4(xf, yf, -1.f, 1.f);
	return glm::normalize(ray0 - worldOriginInverse);
}

glm::vec3 Camera::generateRay(float x, float y) {
	//float x and y in NDC, -1 to 1; y value is flipped since screen space is upside down
	float xf = (2.f * x / (float)width - 1.f) * tangent * aspectRatio;
	float yf = (1.f - 2.f * y / (float)height) * tangent;
	glm::vec4 ray0 = viewInverse * glm::vec4(xf, yf, -1.f, 1.f);
	return glm::normalize(ray0 - worldOriginInverse);
}

