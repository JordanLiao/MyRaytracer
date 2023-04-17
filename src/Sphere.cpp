#include "Sphere.h"

Sphere::Sphere() : Sphere(glm::vec3(0.0f), 1.f, new Resources::Material()) {}

Sphere::Sphere(const glm::vec3 & position, float radius, Resources::Material* mtl) {
	this->material = mtl;
	this->position = position;
	this->radius = radius;
}

Sphere::~Sphere() {
	delete material;
}

float Sphere::intersect(const glm::vec3& raySource, const glm::vec3& ray) {
	//refer to this for analytical solution https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
	glm::vec3 L = raySource - position;
	float b = 2.f * glm::dot(ray, L);
	float c = glm::dot(L, L) - radius * radius;
    float t0, t1;
    if (solveQuadratic(1.f, b, c, t0, t1)) {
        if (t0 > t1) 
            std::swap(t0, t1);
        if (t0 < 0.001f) {
            t0 = t1; // if t0 is negative, let's use t1 instead
            if (t0 <= 0.001f) 
                return FLT_MAX; // both t0 and t1 are negative
        }
        return t0;
    }    
    else
        return FLT_MAX;
}

bool Sphere::solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1) {
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = -0.5 * b / a;
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);

    return true;
}
