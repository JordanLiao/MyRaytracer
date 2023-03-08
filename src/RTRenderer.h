#ifndef _RTRENDERER_H_ 
#define _RTRENDERER_H_

#include "Camera.h"
#include "Scene.h"
#include "Triangle.h"
#include "tgaimage.h"

class RTRenderer {
public:
	static void render(TGAImage* outputFrame, Scene* scene, Camera& cam);
private:
	/*
		compute the color for one pixel
	*/
	static void castRay(TGAColor* outputColor, glm::vec3& ray, Scene* scene, Camera& cam);

	/*
		finds the nearest triangle to the camera that intersects with the ray; if there is no such a triangle return null
	*/
	static Triangle* trace(glm::vec3& raySource, glm::vec3& ray, float& rayLength, std::vector<Object*> objects);

	/*
		simple pixel shader
	*/
	static void shade(TGAColor* outputColor, glm::vec3& raySource, glm::vec3& ray, glm::vec3& intersect, Triangle* triangle, Scene* scene);
};

#endif
