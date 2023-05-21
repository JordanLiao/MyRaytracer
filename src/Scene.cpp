#include "Scene.h"
#include <iostream>
#include <chrono>

Scene::Scene() {
	//light = Light(glm::vec3(0.f, 5.f, 5.f), glm::vec3(0.9f, 0.9f, 0.9f));
	quadlight = QuadLight(glm::vec3(-0.6f, 1.5f, -0.6f), glm::vec3(-0.6f, 1.5f, 0.6f), glm::vec3(0.6f, 1.5f, 0.6f), glm::vec3(12.5f));
	objects.push_back(quadlight.obj);
}

void Scene::setup() {
	objects.push_back(ResourceManager::loadObject("Assets/cornell_box.fbx", false));
	
	Object * sphereObj = new Object("Sphere", glm::mat4(1.f));
	sphereObj->sphere = new Sphere(glm::vec3(0.f, 0.5f, 0.f), 0.5f, new Resources::Material());
	sphereObj->sphere->material->diffuse = glm::vec3(0.1f);
	sphereObj->sphere->material->specular = glm::vec3(0.9f);
	sphereObj->sphere->material->specularFocus = 1000;
	//objects.push_back(sphereObj);

	Object* bunny = ResourceManager::loadObject("Assets/bunny.fbx", true);
	bunny->mdf->mtl->diffuse = glm::vec3(0.2f, 0.15f, 0.0f);
	bunny->mdf->mtl->specular = glm::vec3(0.99f, 0.8f, 0.0f);
	bunny->mdf->mtl->specularFocus = 20.f;
	objects.push_back(bunny);
	std::cout << "finished setting up the scene" << std::endl;
}
