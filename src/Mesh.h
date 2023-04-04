#ifndef _MESH_H_
#define _MESH_H_

#include <string>
#include <vector>

#include "Triangle.h"
#include "Resources.h"

class Mesh {
public:
	std::string meshName;
	Resources::Material* material;
	std::vector<Triangle> triangles;

	Mesh(const std::string& name, Resources::Material* mtl);
	~Mesh();
};

#endif
