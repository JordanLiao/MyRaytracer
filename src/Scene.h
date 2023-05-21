#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include "Triangle.h"
#include "QuadLight.h"
#include "ResourceManager.h"

class Scene {
public:
	QuadLight quadlight;
	std::vector<Object*> objects;

	Scene();
	void setup();
};

#endif
