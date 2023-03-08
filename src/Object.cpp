#include "Object.h"

Object::Object(std::string& objFileName, glm::mat4 model) {
    this->objFileName = objFileName;
    this->model = model;
}

Object::~Object() {
    size_t size = meshList.size();
    for (size_t i = 0; i < size; i++)
        delete meshList[i];
}