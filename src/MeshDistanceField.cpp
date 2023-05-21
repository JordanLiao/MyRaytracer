#include "MeshDistanceField.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include "GLM/ext.hpp"

MeshDistanceField::MeshDistanceField(const float& width, const float& height, const float& depth, const float& resUnit, 
	                                 const glm::vec3& pos, Resources::Material *mtl) {
	if (width < 0.f || height < 0.f || depth < 0.f)
		throw std::invalid_argument("Invalid Dimension: one of the dimensions is negative.");

	this->resUnit = resUnit;
	this->mtl = mtl;
	this->pos = pos;

	resW = calculateResolution(width, resUnit);
	resH = calculateResolution(height, resUnit);
	resD = calculateResolution(depth, resUnit);
	dimensions = glm::vec3(resW * resUnit, resH * resUnit, resD * resUnit);
	maxDim = glm::max(dimensions.x, dimensions.y, dimensions.z);
	lowerPos = pos - glm::vec3(dimensions.x, dimensions.y, -1.0f * dimensions.z) / 2.0f;
	upperPos = lowerPos + glm::vec3(dimensions.x, dimensions.y, -1.0f * dimensions.z);
	
	mdf.resize(resW * resH * resD, FLT_MAX);
	normals.resize(resW * resH * resD, glm::vec3(FLT_MAX));
	mdfBoundary.reserve(resW * resH * resD);
}

float MeshDistanceField::intersect(glm::vec3& hitNormal, const glm::vec3& raySource, const glm::vec3& ray) {
	if (isInside(raySource)) { //if already inside the MDF AABB, then there is no external distance.
		return internalIntersect(hitNormal, raySource + INTERSECT_DIST_THRESHOLD * resUnit * ray, ray);
	}

	float tmin = (lowerPos.x - raySource.x) / ray.x;
	float tmax = (upperPos.x - raySource.x) / ray.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (lowerPos.y - raySource.y) / ray.y;
	float tymax = (upperPos.y - raySource.y) / ray.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return FLT_MAX;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (lowerPos.z - raySource.z) / ray.z;
	float tzmax = (upperPos.z - raySource.z) / ray.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return FLT_MAX;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tmin < 0.f)
		return FLT_MAX;

	glm::vec3 intersect = raySource + (tmin + resUnit / 2.f) * ray; //entry point starts a little inside the MDF AABB
	float internalDist = internalIntersect(hitNormal, intersect, ray);
	if (internalDist == FLT_MAX)
		return FLT_MAX;
	return internalDist + glm::length(intersect - raySource);
}

float MeshDistanceField::internalIntersect(glm::vec3& hitNormal, const glm::vec3& raySource, const glm::vec3& ray) {
	glm::vec3 stepPos = raySource;
	glm::vec3 normal(0.f);
	float dist = interpolate(normal, stepPos);
	float intersectThreshold = INTERSECT_DIST_THRESHOLD * resUnit;
	//while (glm::length(stepPos - pos) < maxDim / 1.9f) {
	while (isInside(stepPos)) {
		if (dist < intersectThreshold) {
			hitNormal = normal;
			return dist;
		}
		stepPos = stepPos + dist * ray;
		dist = interpolate(normal, stepPos);
	}

	return FLT_MAX;
}

bool MeshDistanceField::isInside(const glm::vec3& stepPos) {
	if (stepPos.x < lowerPos.x || stepPos.x > upperPos.x || stepPos.y < lowerPos.y || stepPos.y > upperPos.y ||
		stepPos.z > lowerPos.z || stepPos.z < upperPos.z)
		return false;
	return true;
}

float MeshDistanceField::interpolate(glm::vec3& normal, const glm::vec3& stepPos) {
	glm::vec3 unsignedStepPos = glm::abs(stepPos - lowerPos); //unsigned step pos relative to lower corner of the MDF
	glm::vec3 center = unsignedStepPos / resUnit - glm::vec3(0.5f);
	int lx = std::min((int)(std::floorf(center.x)), resW - 2);
	int ly = std::min((int)(std::floorf(center.y)), resH - 2);
	int lz = std::min((int)(std::floorf(center.z)), resH - 2);

	glm::vec3 c000p = (glm::vec3(lx, ly, lz) + 0.5f) * resUnit;

	/*float d0 = getDistDefaultZero(lx, ly, lz);
	float d1 = getDistDefaultZero(lx + 1, ly, lz);
	float d2 = getDistDefaultZero(lx, ly + 1, lz);
	float d3 = getDistDefaultZero(lx + 1, ly + 1, lz);
	float d4 = getDistDefaultZero(lx, ly, lz + 1);
	float d5 = getDistDefaultZero(lx + 1, ly, lz + 1);
	float d6 = getDistDefaultZero(lx, ly + 1, lz + 1);
	float d7 = getDistDefaultZero(lx + 1, ly + 1, lz + 1);*/


	float xd = std::max((unsignedStepPos.x - c000p.x) / resUnit, 0.f);
	xd = std::min(xd, 1.0f);
	float c00 = getDistDefaultZero(lx, ly, lz) * (1.0f - xd) + getDistDefaultZero(lx + 1, ly, lz) * xd;
	float c01 = getDistDefaultZero(lx, ly + 1, lz) * (1.0f - xd) + getDistDefaultZero(lx + 1, ly + 1, lz) * xd;
	float c10 = getDistDefaultZero(lx, ly, lz + 1) * (1.0f - xd) + getDistDefaultZero(lx + 1, ly, lz + 1) * xd;
	float c11 = getDistDefaultZero(lx, ly + 1, lz + 1) * (1.0f - xd) + getDistDefaultZero(lx + 1, ly + 1, lz + 1) * xd;
	glm::vec3 n00 = getNormal(lx, ly, lz) * (1.0f - xd) + getNormal(lx + 1, ly, lz) * xd;
	glm::vec3 n01 = getNormal(lx, ly + 1, lz) * (1.0f - xd) + getNormal(lx + 1, ly + 1, lz) * xd;
	glm::vec3 n10 = getNormal(lx, ly, lz + 1) * (1.0f - xd) + getNormal(lx + 1, ly, lz + 1) * xd;
	glm::vec3 n11 = getNormal(lx, ly + 1, lz + 1) * (1.0f - xd) + getNormal(lx + 1, ly + 1, lz + 1) * xd;
	

	float yd = std::max((unsignedStepPos.y - c000p.y) / resUnit, 0.f);
	yd = std::min(yd, 1.0f);
	float c0 = c00 * (1.0f - yd) + c01 * yd;
	float c1 = c10 * (1.0f - yd) + c11 * yd;
	glm::vec3 n0 = n00 * (1.0f - yd) + n01 * yd;
	glm::vec3 n1 = n10 * (1.0f - yd) + n11 * yd;

	float zd = std::max((unsignedStepPos.z - c000p.z) / resUnit, 0.f);
	zd = std::min(zd, 1.0f);
	float distInterp = c0 * (1 - zd) + c1 * zd;

	normal = n0 * (1 - zd) + n1 * zd;
	return distInterp;
}  

float MeshDistanceField::getDist(int i, int j, int k) {
	if (i >= resW || i < 0 || j >= resH || j < 0 || k >= resD || k < 0)
		return FLT_MAX;
	return mdf[i * (resH * resD) + j * resD + k];
}

glm::vec3 MeshDistanceField::getNormal(int i, int j, int k) {
	if (i >= resW || i < 0 || j >= resH || j < 0 || k >= resD || k < 0)
		return glm::vec3(FLT_MAX);
	return normals[i * (resH * resD) + j * resD + k];
}

float MeshDistanceField::getDistDefaultZero(int i, int j, int k) {
	if (i >= resW || i < 0 || j >= resH || j < 0|| k >= resD || k < 0)
		return 0.f;
	return mdf[i * (resH * resD) + j * resD + k];
}

void MeshDistanceField::setDist(int i, int j, int k, float dist) {
	if (i >= resW || j >= resH || k >= resD)
		return;
	mdf[i * (resH * resD) + j * resD + k] = dist;
}

void MeshDistanceField::setNormal(int i, int j, int k, const glm::vec3& normal) {
	if (i >= resW || j >= resH || k >= resD)
		return;
	normals[i * (resH * resD) + j * resD + k] = normal;
}

void MeshDistanceField::createVisualization() {
	mdfBoundary.reserve(mdf.size());
	for (int i = 0; i < mdf.size(); i++) {
		int dimW = i / (resH * resD);
		int dimH = i % (resH * resD) / resD;
		int dimD = (i  % (resH * resD)) % resD;
		mdfBoundary.push_back(lowerPos + resUnit * glm::vec3((float)dimW +0.5f, (float)dimH + 0.5f, -1.f * (float)dimD - 0.5f));
	}

	// Generate a Vertex Array (VAO) and Vertex Buffer Object (VBO)
	glGenVertexArrays(1, &vao);
	glGenBuffers(3, vbo);

	// Bind VAO
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mdfBoundary.size(), mdfBoundary.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mdf.size(), mdf.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MeshDistanceField::drawMDFBoundary(GLuint shaderID, const glm::vec3& color, const glm::mat4& projView) {
	glm::mat4 model(1.0);
	// Actiavte the shader program 
	glUseProgram(shaderID);

	// Get the shader variable locations and send the uniform data to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projView"), 1, GL_FALSE, glm::value_ptr(projView));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1f(glGetUniformLocation(shaderID, "threshold"), resUnit);
	//glUniform3fv(glGetUniformLocation(shaderID, "diffuseColor"), 1, glm::value_ptr(color));

	// Bind the VAO
	glBindVertexArray(vao);

	// Set point size
	glPointSize(14);

	// Draw the points 
	glDrawArrays(GL_POINTS, 0, mdfBoundary.size());

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

int MeshDistanceField::calculateResolution(const float & length, const float & resUnit) {
	if (fmod(length / resUnit, 1.0f) > 0.001f)
		return (int)std::ceil(length / resUnit) + 2;
	else
		return (int)(length / resUnit) + 2;
}
