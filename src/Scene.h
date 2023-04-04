#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include "Triangle.h"
#include "Light.h"
#include "QuadLight.h"
#include "ResourceManager.h"

class Scene {
public:
	Light light;
	QuadLight quadlight;
	std::vector<Object*> objects;

	Scene();
	void setup();
};

#endif
