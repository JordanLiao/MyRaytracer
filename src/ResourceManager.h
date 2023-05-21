#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include "Object.h"
#include "RayTracer.h"
#include "GLM/glm.hpp"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "Mesh.h"
#include "Resources.h"
#include "Triangle.h"

#include <vector>
#include <unordered_map>
#include <string>

#define MDF_UNIT_SIZE 0.05f
#define NUM_MDF_SAMPLE 300

class ResourceManager {
public:
	static Object* loadObject(const char* fPath, bool generateMDF);
private:
	//maps object's name(not file path name) to object's address.
	static std::unordered_map<std::string, Object*> objMap;

	/*
		Fill the given vertices vectors, given a pMesh reference.
		returns the max individual values of x,y,z found in the vertices as a vec3
	*/
	static std::pair<glm::vec3, glm::vec3> processMeshVertices(aiMesh* pMesh, std::vector<glm::vec3>& vert, std::vector<glm::vec3>& norm, std::vector<glm::vec2>& text);

	/*
		Generate and fill a triangle vector, given a pMesh reference, and three already processed vertices vectors.
		vertexOffset offsets the pMesh indices into the correct vertices vectors' positions.
	*/
	static void processMeshFaces(std::vector<Triangle>& triangles, aiMesh* pMesh, std::vector<glm::vec3>& vert, std::vector<glm::vec3>& norm,
		                         std::vector<glm::vec2>& text, int vertexOffset);

	static void generateTriangles(std::vector<Triangle*>& triangles, std::vector<glm::ivec3>& indices, std::vector<glm::vec3>& vert, std::vector<glm::vec3>& norm);

	static MeshDistanceField* generateMeshDistanceField(Object* obj);

	static Resources::Material* loadMaterial(const aiMaterial* mtl);

	//extract the file name from the file path
	static std::string getFileNameFromPath(std::string& fPath);

	//extract the prefix folder path /a/b/ from full file path /a/b/file.ext
	static std::string getFolderPath(std::string& fPath);
};

#endif
