#include "RTRenderer.h"

void RTRenderer::render(TGAImage* outputFrame, Scene* scene, Camera& cam) {
	unsigned int width = cam.width, height = cam.height;
	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {
			glm::vec3 ray;
			cam.generateRay(j, i, ray);
			TGAColor pixelColor;
			castRay(&pixelColor, ray, scene, cam);
			outputFrame->set(j, height-i-1, pixelColor);
		}
	}
}

void RTRenderer::castRay(TGAColor* outputColor, glm::vec3& ray, Scene* scene, Camera& cam) {
	float rayLength;
	Triangle* triangle = trace(cam.pos, ray, rayLength, scene->objects);
	if (triangle) { //if a triangle is hit
		glm::vec3 intersect = cam.pos + rayLength * ray;
		glm::vec3 shadowRay = glm::normalize(scene->light.pos - intersect);
		float filler;
		//if there is no obstructing triangle along the shadow ray
		if (!trace(intersect, shadowRay, filler, scene->objects)) {
			shade(outputColor, cam.pos, ray, intersect, triangle, scene);
		}
	}
}

Triangle* RTRenderer::trace(glm::vec3& raySource, glm::vec3& ray, float& rayLength, std::vector<Object*> objects) {
	//naive implementation of tracing without accelaration structure
	float minDist = FLT_MAX;
	Triangle* triangle = nullptr;
	size_t numObj = objects.size();
	for (size_t i = 0; i < numObj; i++) {
		size_t numMesh = objects[i]->meshList.size();
		for (size_t j = 0; j < numMesh; j++) {
			size_t numTriangles = objects[i]->meshList[j]->triangles.size();
			for (size_t k = 0; k < numTriangles; k++) {
				float dist = objects[i]->meshList[j]->triangles[k].intersect(raySource, ray);
				if (dist < minDist) {
					triangle = &(objects[i]->meshList[j]->triangles[k]);
					minDist = dist;
				}
			}
		}
	}
	rayLength = minDist;
	return triangle;
}

void RTRenderer::shade(TGAColor* outputColor, glm::vec3& raySource, glm::vec3& ray, glm::vec3& intersect, Triangle* triangle, Scene* scene) {
	glm::vec3 color = glm::vec3(0.9f, 0.9f, 0.9f);
	glm::vec3 lightDir = glm::normalize(raySource - intersect);
	float diffuse = glm::dot(lightDir, triangle->faceNormal);
	*outputColor = TGAColor(255 * diffuse, 255 * diffuse, 255 * diffuse, 255);
}



