
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <unordered_map>

#include "Mesh.h"
#include "Sphere.h"
#include "Resources.h"
#include "MeshDistanceField.h"
#include "GLM/glm.hpp"

class Object {

public:
	std::string objFileName;
	glm::vec3 pos;
	glm::mat4 model;
	std::vector<Mesh*> meshList; //might want to use a different data structure for efficiencys
	Sphere * sphere;
	MeshDistanceField* mdf;

	float width, height, depth; //dimension of the minimum aabb for this object

	Object(const std::string& objFileName, glm::mat4 model);
	~Object();
};

#endif