#include "Mesh.h"

Mesh::Mesh(std::string& name, Resources::Material* mtl) {
    meshName = name;
    material = mtl;
}

Mesh::~Mesh() {
    delete material;
}

