#include "Triangle.h"

Triangle::Triangle(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& n1, glm::vec3& n2, glm::vec3& n3) {
	positions[0] = p1;
	positions[1] = p2;
	positions[2] = p3;
	faceNormal = glm::normalize(glm::cross(p1 - p2, p1 - p3));
	normals[0] = n1;
	normals[1] = n2;
	normals[2] = n3;

	//fixed factors needed for Barycentric coordinates calculations
	v0 = p2 - p1;
	v1 = p3 - p1;
	d00 = glm::dot(v0, v0);
	d01 = glm::dot(v0, v1);
	d11 = glm::dot(v1, v1);
	denom = d00 * d11 - d01 * d01;
}

float Triangle::intersect(glm::vec3& baryCenter, const glm::vec3& raySource, const glm::vec3& ray) {
	//length of the perpendicular component of the vector from origin to the PLANE that the triangle is on
	float dist = -1.f * (glm::dot(faceNormal, positions[0]));
	float normalDotRay = glm::dot(faceNormal, ray);
	//float t = -1.f * (glm::dot(faceNormal, raySource) + dist) / (glm::dot(faceNormal, ray));
	//vector length from raySource to the PLANE that the triangle is on
	float t = -1.f * (glm::dot(faceNormal, raySource) + dist) / normalDotRay;
	if(t <= 0.001f) //ray is aiming at opposite direction of the triangle
		return FLT_MAX;

	glm::vec3 p = raySource + t * ray; //intersection point of the ray on the plane that the triangle is on
	glm::vec3 v2 = p - positions[0];
	float d20 = glm::dot(v2, v0);
	float d21 = glm::dot(v2, v1);
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.f - v - w;

	if (v < 0.f || w < 0.f || u < 0.f)
		return FLT_MAX;
	else {
		baryCenter = glm::vec3(u, v, w);
		//if (normalDotRay > 0.f) //if ray is coming from the back face of the triangle 
			//t *= -1.f;
		return t;
	}
}

