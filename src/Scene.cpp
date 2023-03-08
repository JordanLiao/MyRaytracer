#include "Scene.h"

Scene::Scene() {
	light = Light(glm::vec3(0.f, 0.f, 10.f), glm::vec3(0.9f, 0.9f, 0.9f));
}

void Scene::setup() {
	objects.push_back(ResourceManager::loadObject("Assets/lowpolypine.obj"));
	//objects.push_back(ResourceManager::loadObject("Assets/triangle.obj"));
}
