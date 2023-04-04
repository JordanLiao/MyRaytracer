#include "Mesh.h"

Mesh::Mesh(const std::string& name, Resources::Material* mtl) {
    meshName = name;
    material = mtl;
}

Mesh::~Mesh() {
    delete material;
}

