#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include "Core.h"
#include <string>

namespace Resources {
	struct Material {
		std::string materialName;
		glm::vec3 ambient; //ka
		glm::vec3 diffuse;  //kd
		glm::vec3 specular;  //ks
		glm::vec3 emissive;  //emissive Coeficient
		GLfloat specularFocus;  //ns
		GLfloat opticalDensity; //ni
		GLfloat nonTransparency;  //d
		GLuint illum;  //type of illumination
		GLuint diffuseTexture; //map_kd

		Material();
	};
};

#endif