#include "Scene.h"

Scene::Scene() {
	//light = Light(glm::vec3(0.f, 5.f, 5.f), glm::vec3(0.9f, 0.9f, 0.9f));
	//quadlight = QuadLight(glm::vec3(-15.f, 5.f, -15.f), glm::vec3(-15.f, 5.f, 15.f), glm::vec3(15.f, 5.f, 15.f), glm::vec3(0.99, 0.99, 0.99));
	float overallRadiance = 8.0f;
	quadlight = QuadLight(glm::vec3(-0.5f, 3.01f, -0.5f), glm::vec3(-0.5f, 3.01f, 0.5f), glm::vec3(0.5f, 3.01f, 0.5f), glm::vec3(overallRadiance));
	objects.push_back(quadlight.obj);
}

void Scene::setup() {
	//objects.push_back(ResourceManager::loadObject("Assets/ground.obj"));
	//objects.push_back(ResourceManager::loadObject("Assets/lowpolypine.obj"));
	//objects.push_back(ResourceManager::loadObject("Assets/sphere.obj"));
	//objects.push_back(ResourceManager::loadObject("Assets/cube.obj"));
	//Object* cornell_box = ResourceManager::loadObject("Assets/cornell_box.fbx");
	objects.push_back(ResourceManager::loadObject("Assets/cornell_box.fbx"));

}
