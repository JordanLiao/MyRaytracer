#include "Resources.h"

Resources::Material::Material() {
	materialName = "default";
	ambient = glm::vec3(0.0f, 0.0f, 0.0f); //ka
	diffuse = glm::vec3(0.5f, 0.5f, 0.5f);  //kd
	specular = glm::vec3(0.f, 0.f, 0.f);  //ks
	emissive = glm::vec3(0);
	specularFocus = 0.f;  //ns
	opticalDensity = 1.f; //ni
	nonTransparency = 1.0f;  //d
	illum = 0;  //type of illumination
	diffuseTexture = 0; //map_kd
}