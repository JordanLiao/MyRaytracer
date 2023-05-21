#include "RTCamera.h"

RTCamera::RTCamera() : RTCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.f), glm::vec3(0.f, 1.f, 0.f), 16.f / 9.f, 
								glm::radians(60.f), 320, 180) {}

RTCamera::RTCamera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up, float aspectRatio, float vfov, unsigned int width, unsigned int height) {
	this->pos = pos;
	this->lookAt = lookAt;
	lookDir = glm::normalize(lookAt - pos);
	this->up = up;
	camLeft = glm::normalize(glm::cross(up, lookDir));
	camRight = camLeft * -1.0f;
	camUp = glm::normalize(glm::cross(lookDir, camLeft));
	camDown = camUp * -1.0f;

	view = glm::lookAt(pos, lookAt, up);
	viewInverse = glm::inverse(view);
	worldOriginInverse = viewInverse * glm::vec4(0.f, 0.f, 0.f, 1.0f);
	this->aspectRatio = aspectRatio;
	this->vfov = vfov;
	tangent = glm::tan(vfov * 0.5f);

	this->width = width;
	this->height = height;
}

glm::vec3 RTCamera::generateCenterAlignedRay(unsigned int x, unsigned int y) {
	//float x and y in NDC, -1 to 1; y value is flipped since screen space is upside down
	float xf = (2.f * ((float)x + 0.5f) / (float)width - 1.f) * tangent * aspectRatio; //plus 0.5 to shoot through the center of a pixel
	float yf = (1.f - 2.f * ((float)y + 0.5f) / (float)height) * tangent;
	glm::vec4 ray0 = viewInverse * glm::vec4(xf, yf, -1.f, 1.f);
	return glm::normalize(ray0 - worldOriginInverse);
}

glm::vec3 RTCamera::generateRay(float x, float y) {
	//float x and y in NDC, -1 to 1; y value is flipped since screen space is upside down
	float xf = (2.f * x / (float)width - 1.f) * tangent * aspectRatio;
	float yf = (1.f - 2.f * y / (float)height) * tangent;
	glm::vec4 ray0 = viewInverse * glm::vec4(xf, yf, -1.f, 1.f);
	return glm::normalize(ray0 - worldOriginInverse);
}

void RTCamera::translate(const glm::vec3& offset) {
	pos += offset;
	lookAt = pos + lookDir;
	updateView();
}

void RTCamera::rotate(const glm::vec3& axis, const float& rad) {
	glm::vec3 temp = glm::normalize(glm::vec4(lookDir, 0.0f) * glm::rotate(rad, axis));
	if (glm::length(glm::dot(temp, glm::vec3(0.f, 1.f, 0.f))) < 0.98f)
		lookDir = temp;
	lookAt = pos + lookDir;
	camLeft = glm::normalize(glm::cross(up, lookAt - pos));
	camRight = camLeft * -1.0f;
	camUp = glm::normalize(glm::cross(lookDir, camLeft));
	camDown = camUp * -1.0f;
	updateView();
}

void RTCamera::updateView() {
	view = glm::lookAt(pos, lookAt, up);
	viewInverse = glm::inverse(view);
	worldOriginInverse = viewInverse * glm::vec4(0.f, 0.f, 0.f, 1.0f);
}

