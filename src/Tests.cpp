#include "Tests.h"

void sphereSamplingTest() {
	glm::vec3 n = glm::normalize(glm::vec3(1.f, 1.f, 1.f));
	glm::vec3 v = glm::normalize(glm::vec3(0.3f, 0.3f, 0.8f));
	float integral = 0.f;
	int iter = 1000;
	for (int i = 0; i < iter; i++) {
		glm::vec3 sample = glm::normalize(RayTracer::getSphericalSample());
		integral += glm::dot(n, sample);
	}
	integral = integral * 2.f * 3.141592f / (float)iter;
	std::cout << "Integral is " << integral << std::endl;
}

void intersectTest(Scene* scene) {
	glm::vec3 start(0.f, 0.4f, 0.0f);
	glm::vec3 ray = glm::normalize(glm::vec3(0.f, 0.f, 1.f));
	Intersect intersect;
	RayTracer::trace(intersect, start, ray, scene->objects);
}

void mdfIntersectTest(MeshDistanceField* mdf) {
	glm::vec3 start(0.f);
	glm::vec3 ray(0.f, 0.f, -1.f);
	//mdf->intersect(start, ray);
}

void mdfExternalIntersectTest() {
	MeshDistanceField* mdf = new MeshDistanceField(1.0f, 1.0f, 1.0f, 1.0f, glm::vec3(.0f, .0f, .0f), new Resources::Material());
	glm::vec3 raySource(0.f, 0.f, 5.f);
	glm::vec3 ray(0.f, 0.f, -1.f);
	//std::cout << mdf->intersect(raySource, ray) << std::endl;
	
	raySource = glm::vec3(-5.f, 0.f, 5.f);
	ray = glm::normalize(glm::vec3(1.f, 0.f, -1.f));
	//std::cout << mdf->intersect(raySource, ray) << std::endl;

	raySource = glm::vec3(-5.f, 0.f, 0.f);
	ray = glm::normalize(glm::vec3(1.f, 0.f, 0.f));
	//std::cout << mdf->intersect(raySource, ray) << std::endl;

	raySource = glm::vec3(0.f, 0.f, -5.f);
	ray = glm::normalize(glm::vec3(0.f, 0.f, -1.f));
	//std::cout << mdf->intersect(raySource, ray) << std::endl;

	raySource = glm::vec3(0.f, 0.f, -5.f);
	ray = glm::normalize(glm::vec3(1.f, 0.f, 0.f));
	//std::cout << mdf->intersect(raySource, ray) << std::endl;
}

void mdfInterpolationTest() {
	MeshDistanceField* mdf = new MeshDistanceField(1.0f, 1.0f, 1.0f, 1.0f, glm::vec3(.0f, .0f, .0f), new Resources::Material());

	float distAcc = 0.f;
	for (int i = 0; i < mdf->resW; i++) {
		for (int j = 0; j < mdf->resH; j++) {
			for (int k = 0; k < mdf->resD; k++) {
				mdf->setDist(i, j, k, distAcc++);
			}
		}
	}

	/*std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.f, -1.f, 1.f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(1.f, -1.f, 1.f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.f, -1.f, -1.f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(1.f, -1.f, -1.f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.f, 1.f, 1.f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(1.f, 1.f, 1.f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.f, 1.f, -1.f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(1.f, 1.f, -1.f)) << std::endl;
	std::cout << "\n" << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.1f, -1.1f, 1.1f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(6.1f, -6.1f, 6.1f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.1f, -1.1f, -1.1f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(1.1f, -1.1f, -1.1f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.1f, 1.1f, 1.1f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(1.1f, 1.1f, 1.1f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(-1.1f, 1.1f, -1.1f)) << std::endl;
	std::cout << "MDF interpolation test: " << mdf->interpolate(glm::vec3(1.1f, 1.1f, -1.1f)) << std::endl;*/
}
