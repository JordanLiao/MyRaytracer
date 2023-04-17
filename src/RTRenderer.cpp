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
	renderOnThread(outputFrame, scene, cam, 0, cam.height);
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
	if (trace(intersect, cam.pos, ray, scene->objects)) {
		return shade(ray, intersect, scene);
	}
	return glm::vec3(0.f);
}

bool RTRenderer::trace(Intersect& intersect, const glm::vec3& raySource, const glm::vec3& ray, std::vector<Object*> objects) {
	//naive implementation of tracing without accelaration structure
	float rayLength = FLT_MAX;
	glm::vec3 baryCenter;

	ObjectType objType;
	Triangle* hitTriangle = nullptr;
	Sphere* hitSphere = nullptr;


	size_t numObj = objects.size();
	for (size_t i = 0; i < numObj; i++) {
		size_t numMesh = objects[i]->meshList.size();
		for (size_t j = 0; j < numMesh; j++) {
			size_t numTriangles = objects[i]->meshList[j]->triangles.size();
			for (size_t k = 0; k < numTriangles; k++) {
				glm::vec3 bCenter;
				float dist = objects[i]->meshList[j]->triangles[k].intersect(bCenter, raySource, ray);
				if (dist < rayLength) {
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
	if(objType == ObjectType::triangle){
		intersect.normal = glm::normalize(hitTriangle->normals[0] * baryCenter[0] +
							              hitTriangle->normals[1] * baryCenter[1] +
							              hitTriangle->normals[2] * baryCenter[2]);
	} else {
		intersect.normal = glm::normalize(intersect.point - hitSphere->position);
	}
	return true;
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
	glm::vec3 abStratSize = quad.ab / (float)NUM_QUAD_SAMPLES_SQRT;
	glm::vec3 cbStratSize = quad.cb / (float)NUM_QUAD_SAMPLES_SQRT;
	glm::vec3 integral = glm::vec3(0);
	for (int i = 0; i < NUM_QUAD_SAMPLES_SQRT * NUM_QUAD_SAMPLES_SQRT; i++) {
		glm::vec3 samplePoint = quad.b + (getUniformSample() + i / NUM_QUAD_SAMPLES_SQRT) * abStratSize +
			                             (getUniformSample() + i % NUM_QUAD_SAMPLES_SQRT) * cbStratSize;
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
#ifdef _COSINE_SAMPLING_
			glm::vec3 sampledDir = getCosineWeightedSample(intersect.normal);
			if (trace(obstruction, intersect.point, sampledDir, objects)) {
				indirectRadiance += phongBRDF(sampledDir, -1.f * ray, intersect.normal, intersect.mtl) *
					integrate(depth - 1, obstruction, sampledDir, quad, objects);
			}
#endif
#ifdef _BRDF_SAMPLING_
			float specularMean = (intersect.mtl->specular.r + intersect.mtl->specular.g + intersect.mtl->specular.b) / 3.f;
			float diffuseMean = (intersect.mtl->diffuse.r + intersect.mtl->diffuse.g + intersect.mtl->diffuse.b) / 3.f;
			float reflectiveness = specularMean / (specularMean + diffuseMean);
			glm::vec3 reflection = glm::reflect(ray, intersect.normal);
			glm::vec3 sampledDir = getBRDFImportanceSample(intersect.normal, reflection, reflectiveness, intersect.mtl->specularFocus);
		
			if (trace(obstruction, intersect.point, sampledDir, objects)) {
				float pd = getBRDFPD(sampledDir, intersect.normal, reflection, intersect.mtl->specularFocus, reflectiveness);
				glm::vec3 brdf = phongBRDF(sampledDir, -1.f * ray, intersect.normal, intersect.mtl);
				glm::vec3 Li = integrate(depth - 1, obstruction, sampledDir, quad, objects);
				float dot = glm::dot(sampledDir, intersect.normal);
				indirectRadiance += phongBRDF(sampledDir, -1.f * ray, intersect.normal, intersect.mtl)
					* integrate(depth - 1, obstruction, sampledDir, quad, objects)
				    * glm::dot(sampledDir, intersect.normal) / pd;
			}
#endif
		}
	}
#ifdef _COSINE_SAMPLING_
	indirectRadiance = indirectRadiance * 3.1415f / (float)NUM_INTERSECT_SAMPLES;
#endif
#ifdef _BRDF_SAMPLING_
	indirectRadiance = indirectRadiance / (float)NUM_INTERSECT_SAMPLES;
#endif

	return glm::clamp(radiance + indirectRadiance, glm::vec3(0.f), glm::vec3(1.f));
}

glm::vec3 RTRenderer::phongBRDF(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& normal, Resources::Material* mtl) {
	float reflectDotWi = std::max(glm::dot(glm::reflect(wo, normal), wi), 0.f);
	glm::vec3 result =  mtl->diffuse / 3.14159f + mtl->specular * (mtl->specularFocus + 2) / 6.28318f * 
		   glm::pow(reflectDotWi, mtl->specularFocus);
	return result;
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
}


glm::vec3 RTRenderer::getBRDFImportanceSample(const glm::vec3& normal, const glm::vec3& reflection, float reflectiveness, float specularFocus) {
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
	glm::vec3 dir = toHemisphericalDirection(theta, phi, hemisphereCenterDir);
	return dir;

}

float RTRenderer::getBRDFPD(const glm::vec3& wi, const glm::vec3 & normal, const glm::vec3 & reflection, 
	                       float specularFocus, float reflectiveness) {
	float reflectDotWi = std::max(glm::dot(reflection, wi), 0.f);
	return (1.f - reflectiveness) * glm::dot(normal, wi) / 3.14159f + reflectiveness * (specularFocus + 1.f) / 6.2831f
		   * std::powf(reflectDotWi, specularFocus);
}

glm::vec3 RTRenderer::getHemisphericalSample(const glm::vec3 & normal) {
	float theta = glm::acos(getUniformSample());
	float phi = 6.2831f * getUniformSample();
	return toHemisphericalDirection(theta, phi, normal);
}

glm::vec3 RTRenderer::getCosineWeightedSample(const glm::vec3& normal) {
	float theta = glm::acos(std::sqrtf(getUniformSample()));
	float phi = 6.2831f * getUniformSample();
	return toHemisphericalDirection(theta, phi, normal);
}

float RTRenderer::getUniformSample() {
	return (float)rand() / (float)RAND_MAX;
}

glm::vec3 RTRenderer::toHemisphericalDirection(float theta, float phi, const glm::vec3& normal) {
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


