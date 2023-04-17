#include "Resources.h"

Resources::Material::Material() {
	materialName = "default";
	ambient = glm::vec3(0.0f, 0.0f, 0.0f); //ka
	diffuse = glm::vec3(0.9f, 0.9f, 0.9f);  //kd
	specular = glm::vec3(0.9f, 0.9f, 0.9f);  //ks
	emissive = glm::vec3(0);
	specularFocus = 10.f;  //ns
	opticalDensity = 1.f; //ni
	nonTransparency = 1.0f;  //d
	illum = 3;  //type of illumination
	diffuseTexture = 0; //map_kd
}