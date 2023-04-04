#ifndef _RTRENDERER_H_ 
#define _RTRENDERER_H_

#include "Camera.h"
#include "Scene.h"
#include "Resources.h"
#include "Triangle.h"
#include "tgaimage.h"
#include "QuadLight.h"

#define NUM_SAMPLES_SQRT 8 //number of samples would be NUM_SAMPLES_SQRT ^ 2

#define _MULTITHREADED_
#define NUM_THREADS 12

struct Intersect {
	float distance;
	glm::vec3 point;
	glm::vec3 normal;
	glm::vec3 baryCenter;
	Triangle* triangle;
	Resources::Material * mtl;
};

class RTRenderer {
public:
	static void render(TGAImage& outputFrame, Scene* scene, Camera& cam);
private:
	static void renderOnThread(TGAImage& outputFrame, Scene* scene, Camera& cam, int startRow, int endRow);

	/*
		compute the color for one pixel
	*/
	static void castRay(TGAColor& outputColor, const glm::vec3& ray, Scene* scene, Camera& cam);

	/*
		finds the nearest triangle to the camera that intersects with the ray; if there is no such a triangle return null
	*/
	static bool trace(Intersect& intersect, const glm::vec3& raySource, const glm::vec3& ray, std::vector<Object*> objects);

	/*
		simple pixel shader
	*/
	static void shade(TGAColor& outputColor, const glm::vec3& ray, Intersect& intersect, Scene* scene);


	/*
		Analytic direct lighting shading using a quad light source. No shadow ray tracing needed.
	*/
	static void shadeFromQuadAnalytically(TGAColor& outputColor, const glm::vec3& point,
		                            const glm::vec3& normal, Resources::Material* mtl, QuadLight& quad);

	/*
		shading using quad with Monte Carlo Integration
	*/
	static void shadeFromQuad(TGAColor& outputColor, const Intersect& intersect, const glm::vec3& ray,
		                     QuadLight& quad, int numSamplesSqrt, std::vector<Object*>& objects);

	static glm::vec3 phongBRDF(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& normal, Resources::Material* mtl);
};

#endif
