#ifndef _RTRENDERER_H_ 
#define _RTRENDERER_H_

#include "Camera.h"
#include "Scene.h"
#include "Resources.h"
#include "Triangle.h"
#include "tgaimage.h"
#include "QuadLight.h"

//indirect light sampling method
#define _BRDF_SAMPLING_
//#ifndef _BRDF_SAMPLING_
//#define _COSINE_SAMPLING_
//#endif

#define NUM_QUAD_SAMPLES_SQRT 1 //square root of number of samples from a quad light: NUM_SAMPLES_SQRT ^ 2

#define _MULTI_PIXEL_SAMPLES_
#define NUM_PIXEL_SAMPLES_SQRT 6 //square root of number of samples from a pixel

#ifndef _DEBUG
#define _MULTITHREADED_
#define NUM_THREADS 12
#else
#define NUM_THREADS 1
#endif

#define RT_DEPTH 2 //depth of ray tracing
#define NUM_INTERSECT_SAMPLES 1 //how many secondary rays to sample for each intersection

struct Intersect {
	glm::vec3 point;
	glm::vec3 normal;
	float distance;
	Resources::Material * mtl;

#ifdef _DEBUG
	std::string* name;
#endif
};

enum class ObjectType {
	triangle,
	sphere,
};

class RTRenderer {
public:
	static void render(TGAImage& outputFrame, Scene* scene, Camera& cam);

	static void renderOnThread(TGAImage& outputFrame, Scene* scene, Camera& cam, int startRow, int endRow);

	/*
		compute the color for one pixel
	*/
	static glm::vec3 castRay(const glm::vec3& ray, Scene* scene, Camera& cam);

	/*
		finds the nearest triangle to the camera that intersects with the ray; if there is no such a triangle return null
	*/
	static bool trace(Intersect& intersect, const glm::vec3& raySource, const glm::vec3& ray, std::vector<Object*> objects);

	/*
		simple pixel shader
	*/
	static glm::vec3 shade(const glm::vec3& ray, Intersect& intersect, Scene* scene);

	/*
		shading using quad with Monte Carlo Integration
	*/
	static glm::vec3 integrate(unsigned int depth, const Intersect& intersect, const glm::vec3& ray,
		                     QuadLight& quad, std::vector<Object*>& objects);

	static glm::vec3 phongBRDF(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& normal, Resources::Material* mtl);

	/*
		Analytic direct lighting shading using a quad light source. No shadow ray tracing needed.
	*/
	static glm::vec3 shadeFromQuadAnalytically(const glm::vec3& point,
		                            const glm::vec3& normal, Resources::Material* mtl, QuadLight& quad);

	/*
		convert unit spherical coordinates theta and phi to directional vector on a hemisphere centered 
		around the given normal.
	*/
	static glm::vec3 toHemisphericalDirection(float theta, float phi, const glm::vec3& normal);

	static glm::vec3 getBRDFImportanceSample(const glm::vec3& normal, const glm::vec3& reflection, float reflectiveness, float specularFocus);

	//get the probability density for BRDF importance sampling
	static float getBRDFPD(const glm::vec3& wi, const glm::vec3& normal,
		                   const glm::vec3& reflection, float specularFocus, float reflectiveness);

	/*
		uniformly sample a direction on a hemisphere centered around a given normal 
	*/
	static glm::vec3 getHemisphericalSample(const glm::vec3 & normal);

	static glm::vec3 getCosineWeightedSample(const glm::vec3& normal);

	static float getUniformSample();
};

#endif
