#pragma once

#include "GLM/glm.hpp"
#include "RayTracer.h"
#include "Scene.h"
#include "MeshDistanceField.h"

void sphereSamplingTest();
void intersectTest(Scene* scene);
void mdfIntersectTest(MeshDistanceField* mdf);
void mdfExternalIntersectTest();
void mdfInterpolationTest();