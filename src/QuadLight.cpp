#include "QuadLight.h"

QuadLight::QuadLight() : QuadLight(glm::vec3(-1.f, 0.f, -1.f), glm::vec3(-1.f, 0.f, 1.f), glm::vec3(1.f, 0.f, 1.f), glm::vec3(0.9, 0.9, 0.9)) {}

QuadLight::QuadLight(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 radiance) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = c + (a - b);
	ab = a - b;
	cb = c - b;
	this->radiance = radiance;
	area = glm::length(glm::cross(a-b, c-b));
	normal = glm::normalize(glm::cross(a - b, c - b));

	obj = new Object("Quad_Light", glm::mat4(1));
	Mesh* mesh = new Mesh("Quad_Light_Plane", new Resources::Material());
	mesh->material->emissive = radiance;
	mesh->material->diffuse = radiance;
	//mesh->triangles.push_back(Triangle(this->a, this->b, this->c, normal, normal, normal));
	//mesh->triangles.push_back(Triangle(this->c, this->d, this->a, normal, normal, normal));
	mesh->triangles.push_back(Triangle(this->c, this->b, this->a, normal, normal, normal));
	mesh->triangles.push_back(Triangle(this->c, this->a, this->d, normal, normal, normal));
	obj->meshList.push_back(mesh);
}
