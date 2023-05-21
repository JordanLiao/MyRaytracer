#ifndef _RTRENDERER_H_ 
#define _RTRENDERER_H_

#include "RTCamera.h"
#include "RayTracer.h"
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
#define NUM_PIXEL_SAMPLES_SQRT 7 //square root of number of samples from a pixel

#define RT_DEPTH 3 //depth of ray tracing
#define NUM_INTERSECT_SAMPLES 1 //how many secondary rays to sample for each intersection

class RTRenderer {
public:
	static void render(TGAImage& outputFrame, Scene* scene, RTCamera& cam);

	static void renderOnThread(TGAImage& outputFrame, Scene* scene, RTCamera& cam, unsigned int startRow, unsigned int endRow);

	/*
		compute the color for one pixel
	*/
	static glm::vec3 castRay(const glm::vec3& ray, Scene* scene, RTCamera& cam);

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

	//get the probability density for BRDF importance sampling
	static float getBRDFPD(const glm::vec3& wi, const glm::vec3& normal,
		                   const glm::vec3& reflection, float specularFocus, float reflectiveness);
};

#endif
