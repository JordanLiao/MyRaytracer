#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include "Triangle.h"
#include "Light.h"
#include "ResourceManager.h"

class Scene {
public:
	Light light;
	std::vector<Object*> objects;

	Scene();
	void setup();
};

#endif
