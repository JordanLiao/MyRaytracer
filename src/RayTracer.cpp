#include "RayTracer.h"

bool RayTracer::trace(Intersect& intersect, const glm::vec3& raySource, const glm::vec3& ray, std::vector<Object*> objects) {
	//naive implementation of tracing without accelaration structure
	float rayLength = FLT_MAX;
	glm::vec3 baryCenter;

	ObjectType objType;
	Triangle* hitTriangle = nullptr;
	Sphere* hitSphere = nullptr;
	glm::vec3 hitNormal;

	size_t numObj = objects.size();
	for (size_t i = 0; i < numObj; i++) {
		if (objects[i]->mdf) {
			float dist = objects[i]->mdf->intersect(hitNormal, raySource, ray);
			if (dist < rayLength) {
				objType = ObjectType::MDF;
				rayLength = dist;
				intersect.mtl = objects[i]->mdf->mtl;
				intersect.normal = hitNormal;
			}
			continue;
		}

		size_t numMesh = objects[i]->meshList.size();
		for (size_t j = 0; j < numMesh; j++) {
			size_t numTriangles = objects[i]->meshList[j]->triangles.size();
			for (size_t k = 0; k < numTriangles; k++) {
				//if (k == 10)
					//std::cout << "break" << std::endl;
				glm::vec3 bCenter;
				float dist = objects[i]->meshList[j]->triangles[k].intersect(bCenter, raySource, ray);
				if (std::abs(dist) < rayLength) {
					objType = ObjectType::triangle;
					rayLength = dist;
					baryCenter = bCenter;
					hitTriangle = &(objects[i]->meshList[j]->triangles[k]);
					intersect.mtl = objects[i]->meshList[j]->material;
#ifdef _DEBUG
					intersect.name = &objects[i]->objFileName;
#endif
				}
			}
		}

		if (objects[i]->sphere) {
			float dist = objects[i]->sphere->intersect(raySource, ray);
			if (dist < rayLength) {
				objType = ObjectType::sphere;
				rayLength = dist;
				hitSphere = objects[i]->sphere;
				intersect.mtl = hitSphere->material;
#ifdef _DEBUG
				intersect.name = &objects[i]->objFileName;
#endif
			}
		}
	}

	if (rayLength == FLT_MAX) {
		return false;
	}

	intersect.point = raySource + rayLength * ray;
	intersect.distance = rayLength;
	if (objType == ObjectType::triangle) {
		intersect.normal = glm::normalize(hitTriangle->normals[0] * baryCenter[0] +
			hitTriangle->normals[1] * baryCenter[1] +
			hitTriangle->normals[2] * baryCenter[2]);
		intersect.hasNormal = true;
	}
	else if(objType == ObjectType::sphere){
		intersect.normal = glm::normalize(intersect.point - hitSphere->position);
		intersect.hasNormal = true;
	}
	return true;
}

glm::vec3 RayTracer::getHemisphericalSample(const glm::vec3& normal) {
	float theta = glm::acos(getUniformSample());
	float phi = 6.2831f * getUniformSample();
	return toHemisphericalDirection(theta, phi, normal);
}

glm::vec3 RayTracer::getSphericalSample() {
	float theta = glm::acos(1.f - 2.f * getUniformSample());
	float phi = 6.2831f * getUniformSample();
	
	float sinTheta = glm::sin(theta);
	float cosTheta = glm::cos(theta);
	float sinPhi = glm::sin(phi);
	float cosPhi = glm::cos(phi);

	return glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
}

glm::vec3 RayTracer::getBRDFImportanceSample(const glm::vec3& normal, const glm::vec3& reflection, float reflectiveness, float specularFocus) {
	float theta;
	glm::vec3 hemisphereCenterDir; //sampled hemisphere's center vector
	if (getUniformSample() <= reflectiveness) {
		theta = glm::acos(std::powf(getUniformSample(), 1.f / (specularFocus + 1.f)));
		hemisphereCenterDir = reflection;
	}
	else {
		theta = glm::acos(std::sqrtf(getUniformSample()));
		hemisphereCenterDir = normal;
	}
	float phi = 6.2831f * getUniformSample();
	return toHemisphericalDirection(theta, phi, hemisphereCenterDir);
}

glm::vec3 RayTracer::getCosineWeightedSample(const glm::vec3& normal) {
	float theta = glm::acos(std::sqrtf(getUniformSample()));
	float phi = 6.2831f * getUniformSample();
	return toHemisphericalDirection(theta, phi, normal);
}

float RayTracer::getUniformSample() {
	return (float)rand() / (float)RAND_MAX;
}

glm::vec3 RayTracer::toHemisphericalDirection(float theta, float phi, const glm::vec3& normal) {
	float sinTheta = glm::sin(theta);
	float cosTheta = glm::cos(theta);
	float sinPhi = glm::sin(phi);
	float cosPhi = glm::cos(phi);
	//the hemisphere's y-axis is same as the given normal
	glm::vec3 up = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 xh = glm::normalize(glm::cross(up, normal)); //hemisphere's x-axix
	glm::vec3 zh = glm::cross(normal, xh); //hemisphere's z-axix
	//a hemispherical direction in standard basis is (cosPhi * sinTheta, sinPhi * sinTheta, cosTheta)
	return cosPhi * sinTheta * xh + sinPhi * sinTheta * zh + cosTheta * normal;
}
