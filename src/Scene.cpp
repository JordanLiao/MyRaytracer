#include "Scene.h"

Scene::Scene() {
	//light = Light(glm::vec3(0.f, 5.f, 5.f), glm::vec3(0.9f, 0.9f, 0.9f));
	//quadlight = QuadLight(glm::vec3(-15.f, 5.f, -15.f), glm::vec3(-15.f, 5.f, 15.f), glm::vec3(15.f, 5.f, 15.f), glm::vec3(0.99, 0.99, 0.99));
	quadlight = QuadLight(glm::vec3(-0.6f, 3.01f, -0.6f), glm::vec3(-0.6f, 3.01f, 0.6f), glm::vec3(0.6f, 3.01f, 0.6f), glm::vec3(12.5f));
	objects.push_back(quadlight.obj);
}

void Scene::setup() {
	objects.push_back(ResourceManager::loadObject("Assets/cornell_box.fbx"));

	Object * sphereObj = new Object("Sphere", glm::mat4(1.f));
	sphereObj->sphere = new Sphere(glm::vec3(0.f, 0.5f, 0.f), 0.5f, new Resources::Material());
	sphereObj->sphere->material->diffuse = glm::vec3(0.5f);
	sphereObj->sphere->material->specular = glm::vec3(0.95f);
	sphereObj->sphere->material->specularFocus = 120;
	objects.push_back(sphereObj);
	//objects[1]->meshList[6]->material->diffuse = glm::vec3(0.01);
	//objects[1]->meshList[6]->material->specular = glm::vec3(0.99);
	//objects[1]->meshList[6]->material->specularFocus = 5.f;
}
