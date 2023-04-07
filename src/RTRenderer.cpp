#include "RTRenderer.h"

#include <chrono>
#include <thread>

void RTRenderer::render(TGAImage& outputFrame, Scene* scene, Camera& cam) {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#ifdef _MULTITHREADED_
	std::vector<std::thread> threads;
	int rowPerThread = cam.height / NUM_THREADS;
	for (int i = 0; i < NUM_THREADS-1; i++) {
		threads.emplace_back(std::thread(RTRenderer::renderOnThread, std::ref(outputFrame), scene, std::ref(cam), i * rowPerThread, (i + 1) * rowPerThread));
	}
	threads.emplace_back(std::thread(RTRenderer::renderOnThread, std::ref(outputFrame), scene, std::ref(cam), (NUM_THREADS-1) * rowPerThread, cam.height));

	for (int i = 0; i < NUM_THREADS; i++)
		threads[i].join();
#else
	unsigned int width = cam.width, height = cam.height;
	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {
			glm::vec3 color = castRay(cam.generateRay(j, i), scene, cam);
			TGAColor pixelColor = TGAColor(255 * color.r, 255 * color.g, 255 * color.b, 255);
			outputFrame.set(j, height - i - 1, pixelColor);
		}
	}
#endif

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " 
		      << (double)std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 
		      << " sec" << std::endl;
}

void RTRenderer::renderOnThread(TGAImage& outputFrame, Scene* scene, Camera& cam, int startRow, int endRow) {
	unsigned int width = cam.width, height = cam.height;
	float pixelStratSize = 1.f / (float)NUM_PIXEL_SAMPLES_SQRT; //dim size of a pixel sample
	for (unsigned int i = startRow; i < endRow; i++) {
		for (unsigned int j = 0; j < width; j++) {
			glm::vec3 color = glm::vec3(0.f);
#ifdef _MULTI_PIXEL_SAMPLES_
			for (unsigned int k = 0; k < NUM_PIXEL_SAMPLES_SQRT * NUM_PIXEL_SAMPLES_SQRT; k++) {
				float u1 = ((float)rand() / (float)RAND_MAX + k / NUM_PIXEL_SAMPLES_SQRT) * pixelStratSize; //pixel internal stratifications
				float u2 = ((float)rand() / (float)RAND_MAX + k % NUM_PIXEL_SAMPLES_SQRT) * pixelStratSize;
				color += castRay(cam.generateRay((float)j + u1, (float)i + u2), scene, cam);
			}
			color = color / (float)(NUM_PIXEL_SAMPLES_SQRT * NUM_PIXEL_SAMPLES_SQRT);
#else
			color = castRay(cam.generateRay(j, i), scene, cam);
#endif
			TGAColor pixelColor = TGAColor(255 * color.r, 255 * color.g, 255 * color.b, 255);
			outputFrame.set(j, height - i - 1, pixelColor);
		}
	}
}

glm::vec3 RTRenderer::castRay(const glm::vec3& ray, Scene* scene, Camera& cam) {
	Intersect intersect;
	if (trace(intersect, cam.pos, ray, scene->objects)) { //if a triangle is hit
		return shade(ray, intersect, scene);
	}
	return glm::vec3(0.f);
}

bool RTRenderer::trace(Intersect& intersect, const glm::vec3& raySource, const glm::vec3& ray, std::vector<Object*> objects) {
	//naive implementation of tracing without accelaration structure
	float rayLength = FLT_MAX;
	size_t numObj = objects.size();
	for (size_t i = 0; i < numObj; i++) {
		size_t numMesh = objects[i]->meshList.size();
		for (size_t j = 0; j < numMesh; j++) {
			size_t numTriangles = objects[i]->meshList[j]->triangles.size();
			for (size_t k = 0; k < numTriangles; k++) {
				glm::vec3 bCenter;
				float dist = objects[i]->meshList[j]->triangles[k].intersect(bCenter, raySource, ray);
				if (dist < rayLength) {
					rayLength = dist;
					intersect.baryCenter = bCenter;
					intersect.triangle = &(objects[i]->meshList[j]->triangles[k]);
					intersect.mtl = objects[i]->meshList[j]->material;
				}
			}
		}
	}

	if (rayLength == FLT_MAX)
		return false;
	else {
		intersect.point = raySource + rayLength * ray;
		intersect.distance = rayLength;
		intersect.normal = glm::normalize(intersect.triangle->normals[0] * intersect.baryCenter[0] +
							intersect.triangle->normals[1] * intersect.baryCenter[1] +
							intersect.triangle->normals[2] * intersect.baryCenter[2]);
		return true;
	}
}

glm::vec3 RTRenderer::shade(const glm::vec3& ray, Intersect& intersect, Scene* scene) {
	return integrate(RT_DEPTH, intersect, ray, scene->quadlight, scene->objects);
}

glm::vec3 RTRenderer::integrate(unsigned int depth, const Intersect& intersect, const glm::vec3& ray, 
								QuadLight& quad, std::vector<Object*>& objects) {
	glm::vec3 radiance = glm::vec3(0.0f);
	if (depth == RT_DEPTH)
		radiance = intersect.mtl->emissive;

	//direct lighting
	Intersect obstruction;
	glm::vec3 integral = glm::vec3(0);
	glm::vec3 abStratSize = quad.ab / (float)NUM_QUAD_SAMPLES_SQRT;
	glm::vec3 cbStratSize = quad.cb / (float)NUM_QUAD_SAMPLES_SQRT;
	for (int i = 0; i < NUM_QUAD_SAMPLES_SQRT * NUM_QUAD_SAMPLES_SQRT; i++) {
		float u1 = (float)rand() / (float)RAND_MAX;
		float u2 = (float)rand() / (float)RAND_MAX;
		glm::vec3 samplePoint = quad.b + (u1 + i / NUM_QUAD_SAMPLES_SQRT) * abStratSize + 
			                             (u2 + i % NUM_QUAD_SAMPLES_SQRT) * cbStratSize;
		glm::vec3 incidence = glm::normalize(samplePoint - intersect.point);
		float lightDist = glm::length(samplePoint - intersect.point);
		if (!trace(obstruction, intersect.point, incidence, objects) || (lightDist < obstruction.distance + 0.00001f)) {
			integral += phongBRDF(incidence, -1.f * ray, intersect.normal, intersect.mtl) * 
				    glm::dot(intersect.normal, incidence) * //n . w_i
				    glm::dot(-1.f * quad.normal, incidence) //-n_l . w_i
					/ (lightDist * lightDist);
		}
	}
	radiance = radiance + quad.area / (NUM_QUAD_SAMPLES_SQRT * NUM_QUAD_SAMPLES_SQRT) * integral * quad.radiance;

	//indirect lighting
	glm::vec3 indirectRadiance = glm::vec3(0);
	if (depth != 1) {
		for (int i = 0; i < NUM_INTERSECT_SAMPLES; i++) {
			glm::vec3 sampledDir = getHemisphericalSample(intersect.normal);
			if (trace(obstruction, intersect.point, sampledDir, objects)) {
				indirectRadiance = indirectRadiance + phongBRDF(sampledDir, -1.f * ray, intersect.normal, intersect.mtl) * 
					               integrate(depth - 1, obstruction, sampledDir, quad, objects) *
					               glm::dot(intersect.normal, sampledDir);
			}
		}
	}
	indirectRadiance = indirectRadiance * 6.2831f / (float)NUM_INTERSECT_SAMPLES;

	return glm::clamp(radiance + indirectRadiance, glm::vec3(0.f), glm::vec3(1.f));
}

glm::vec3 RTRenderer::phongBRDF(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& normal, Resources::Material* mtl) {
	float reflectDotWi = std::max(glm::dot(glm::reflect(wo, normal), wi), 0.f);
	return mtl->diffuse / 3.14159f + mtl->specular * (mtl->specularFocus + 2) / 6.28318f * 
		   glm::pow(reflectDotWi, mtl->specularFocus);
}

glm::vec3 RTRenderer::shadeFromQuadAnalytically(const glm::vec3& point,
	const glm::vec3& normal, Resources::Material* mtl, QuadLight& quad) {
	//refer to https://online.ucsd.edu/assets/courseware/v1/10779c35059482f9ad0165dcbb66a67f/asset-v1:CSE+168X+2020-SP+type@asset+block/lambertsformula.png
	glm::vec3 toA = quad.a - point;
	glm::vec3 toB = quad.b - point;
	glm::vec3 toC = quad.c - point;
	glm::vec3 toD = quad.d - point;
	float theta0 = glm::acos(glm::dot(glm::normalize(toA), glm::normalize(toB)));
	float theta1 = glm::acos(glm::dot(glm::normalize(toB), glm::normalize(toC)));
	float theta2 = glm::acos(glm::dot(glm::normalize(toC), glm::normalize(toD)));
	float theta3 = glm::acos(glm::dot(glm::normalize(toD), glm::normalize(toA)));
	glm::vec3 gamma0 = glm::normalize(glm::cross(toA, toB));
	glm::vec3 gamma1 = glm::normalize(glm::cross(toB, toC));
	glm::vec3 gamma2 = glm::normalize(glm::cross(toC, toD));
	glm::vec3 gamma3 = glm::normalize(glm::cross(toD, toA));
	glm::vec3 phi = (theta0 * gamma0 + theta1 * gamma1 + theta2 * gamma2 + theta3 * gamma3) / 2.f;

	glm::vec3 lambertianBRDF = mtl->diffuse / 3.1415f;
	glm::vec3 color = lambertianBRDF * quad.radiance * glm::dot(normal, phi);
	return glm::clamp(color, glm::vec3(0.f), glm::vec3(1.f));
	//outputColor = TGAColor(255 * color.r, 255 * color.g, 255 * color.b, 255);
}

glm::vec3 RTRenderer::getHemisphericalSample(const glm::vec3 & normal) {
	//translation from uniform sampling to spherical sampling
	float theta = glm::acos((float)rand() / (float)RAND_MAX);
	float phi = 6.2831f * (float)rand() / (float)RAND_MAX;
	//
	float sinTheta = glm::sin(theta);
	float cosTheta = glm::cos(theta);
	float sinPhi = glm::sin(phi);
	float cosPhi = glm::cos(phi);
	//the hemisphere's y-axis is same as the given normal
	glm::vec3 xh = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), normal)); //hemisphere's x-axix
	glm::vec3 zh = glm::normalize(glm::cross(normal, xh)); //hemisphere's z-axix
	//a hemispherical direction in standard basis is (cosPhi * sinTheta, sinPhi * sinTheta, cosTheta)
	return glm::normalize(cosPhi * sinTheta * xh + sinPhi * sinTheta * normal + cosTheta * zh);
}


