
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <unordered_map>

#include "Core.h"
#include "Mesh.h"
#include "Resources.h"

class Object {

public:
	std::string objFileName;
	glm::mat4 model;
	std::vector<Mesh*> meshList; //might want to use a different data structure for efficiencys

	Object(std::string& objFileName, glm::mat4 model);
	~Object();
};

#endif