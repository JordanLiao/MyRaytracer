#include "ResourceManager.h"

std::unordered_map<std::string, Object*> ResourceManager::objMap;

Object* ResourceManager::loadObject(const char* fPath) {
	std::string pathName = std::string(fPath);
	std::string objName = std::string(getFileNameFromPath(pathName));

	if (objMap.find(objName) != objMap.end()) {
		return objMap[objName];
	}

	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(fPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals 
		| aiProcess_JoinIdenticalVertices | aiProcess_FixInfacingNormals | aiProcess_PreTransformVertices);
	if (pScene == nullptr) {
		printf("Error parsing '%s': '%s'\n", fPath, imp.GetErrorString());
		return nullptr;
	}

	Object* obj = new Object(objName, glm::mat4(1.0f));

	std::vector<glm::vec3> vert, norm; //vertices and normal values of meshes
	std::vector<glm::vec2> text; //texture coordinates of meshes

	int vertexOffset = 0; //denotes the boundary in "vert" where a mesh begins
	int indexOffset = 0; //denotes the boundary in "indices" where a mesh begins

	//Parse the mesh data of the model/scene, including vertex data, bone data etc.
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		aiMesh* pMesh = pScene->mMeshes[i];
		std::string meshName = std::string(pMesh->mName.C_Str());
		Mesh* mesh = new Mesh(meshName, loadMaterial(pScene->mMaterials[pMesh->mMaterialIndex]));

		processMeshVertices(pMesh, vert, norm, text);
		processMeshFaces(mesh->triangles, pMesh, vert, norm, text, vertexOffset);

		indexOffset = indexOffset + pMesh->mNumFaces * 3;
		vertexOffset = vertexOffset + pMesh->mNumVertices;
		obj->meshList.push_back(mesh);
	}

	return obj;
}

void ResourceManager::processMeshVertices(aiMesh* pMesh, std::vector<glm::vec3>& vert, std::vector<glm::vec3>& norm, std::vector<glm::vec2>& text) {
	aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (unsigned int j = 0; j < pMesh->mNumVertices; j++) {
		vert.push_back(glm::vec3(pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z));
		//because "aiProcess_GenSmoothNormals" flag above we can always explect normals
		norm.push_back(glm::vec3(pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z));
		aiVector3D textCoord = pMesh->HasTextureCoords(0) ? pMesh->mTextureCoords[0][j] : Zero3D;
		text.push_back(glm::vec2(textCoord.x, textCoord.y));
	}
}

void ResourceManager::processMeshFaces(std::vector<Triangle>& triangles, aiMesh* pMesh, std::vector<glm::vec3>& vert, std::vector<glm::vec3>& norm, 
	                                   std::vector<glm::vec2>& text, int vertexOffset) {
	triangles.reserve(pMesh->mNumFaces);
	for (unsigned int j = 0; j < pMesh->mNumFaces; j++) {
		aiFace face = pMesh->mFaces[j];
		//need to offset the index by the number of vertices in the previous meshes because the indices for a mesh
		//are local to a mesh, so they all start from 0.
		int a = face.mIndices[0] + vertexOffset, b = face.mIndices[1] + vertexOffset, c = face.mIndices[2] + vertexOffset;
		glm::vec3 p1 = vert[a];
		glm::vec3 p2 = vert[b];
		glm::vec3 p3 = vert[c];
		glm::vec3 n1 = norm[a];
		glm::vec3 n2 = norm[b];
		glm::vec3 n3 = norm[c];
		triangles.push_back(Triangle(p1, p2, p3, n1, n2, n3));
	}
}

void ResourceManager::generateTriangles(std::vector<Triangle*>& triangles, std::vector<glm::ivec3>& indices, std::vector<glm::vec3>& vert, 
	                                   std::vector<glm::vec3>& norm) {
	size_t size = indices.size();
	for (size_t i = 0; i < size; i++) {
		int a = indices[i].x, b = indices[i].y, c = indices[i].z;
		glm::vec3 p1 = vert[a];
		glm::vec3 p2 = vert[b];
		glm::vec3 p3 = vert[c];
		glm::vec3 n1 = norm[a];
		glm::vec3 n2 = norm[b];
		glm::vec3 n3 = norm[c];
		triangles.push_back(new Triangle(p1, p2, p3, n1, n2, n3));
	}
}

Resources::Material* ResourceManager::loadMaterial(const aiMaterial * mtl) {
	Resources::Material * mat = new Resources::Material();
	mat->materialName = std::string(mtl->GetName().C_Str());

	aiColor4D color;
	float val;
	int intVal;
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &color))
		mat->ambient = glm::vec3(color.r, color.g, color.b);

	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
		mat->diffuse = glm::vec3(color.r, color.g, color.b);

	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
		mat->specular = glm::vec3(color.r, color.g, color.b);

	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &color))
		mat->emissive = glm::vec3(color.r, color.g, color.b);

	if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &val))
		mat->specularFocus = val;

	if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_OPACITY, &val))
		mat->nonTransparency = val;

	if (AI_SUCCESS == aiGetMaterialInteger(mtl, AI_MATKEY_SHADING_MODEL, &intVal))
		mat->illum = intVal;

	return mat;
} 

std::string ResourceManager::getFileNameFromPath(std::string& fPath) {
	return fPath.substr(fPath.rfind("/") + 1, std::string::npos);
}

std::string ResourceManager::getFolderPath(std::string& fPath) {
	return fPath.substr(0, fPath.rfind("/") + 1);
}