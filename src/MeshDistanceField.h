#ifndef _MESHDISTANCEFIELD_H_
#define _MESHDISTANCEFIELD_H_

#include <vector>
#include "GLM/glm.hpp"
#include "Core.h"
#include "Resources.h"

#define INTERSECT_DIST_THRESHOLD 0.1f //factor of resUnit within which a ray would be considered to have intersected

class MeshDistanceField {
public:
	glm::vec3 pos; //center pos of the MDF
	glm::vec3 lowerPos, upperPos;
	//the size of the MDF bound is bigger than the actual obj AABB itself
	glm::vec3 dimensions; //width, height, depth
	float maxDim; //half of the max dimension
	int resW, resH, resD;
	float resUnit;
	std::vector<float> mdf;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> mdfBoundary;

	Resources::Material* mtl;

	//constructor takes in the size of the object aabb and a resolution unit size
	MeshDistanceField(const float & width, const float & height, const float & depth, const float& resUnit, const glm::vec3& pos, Resources::Material * mtl);
	
	//if out of bound return max float(FLT_MAX)
	float getDist(int i, int j, int k);
	glm::vec3 getNormal(int i, int j, int k);
	//if out of bound return max float(FLT_MAX)
	inline float getDistDefaultZero(int i, int j, int k);
	void setDist(int i, int j, int k, float dist);
	void setNormal(int i, int j, int k, const glm::vec3& normal);
	void createVisualization();
	void drawMDFBoundary(GLuint shaderID, const glm::vec3& color, const glm::mat4& projView);

	float intersect(glm::vec3& hitNormal, const glm::vec3& raySource, const glm::vec3& ray);
	//calculate the interpolated normal and distance at this step position
	float interpolate(glm::vec3& normal, const glm::vec3& stepPos);

private:
	GLuint vao;
	GLuint vbo[3];
	float internalIntersect(glm::vec3& hitNormal, const glm::vec3& raySource, const glm::vec3& ray);
	bool isInside(const glm::vec3& stepPos);
	int calculateResolution(const float & length, const float & resUnit);
};

#endif
