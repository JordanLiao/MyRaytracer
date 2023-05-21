#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <string>

#include "Core.h"

enum class shaderType { 
	vertex, 
	fragment 
};

enum shaderRole {
	simpleModelShader,
	phongShader,
	phongShadowShader,
	colorPickingShader,
	shadowMapShader,
	shadowInspectionShader,
	skeletalAnimationShader,
	shaderSize
};

class Shader {
private: 
	static Shader* currBoundShader;
	GLuint programID;
	std::unordered_map<std::string, GLint> uniformMap;

	//methods
	GLuint LoadSingleShader(const char* shaderFilePath, shaderType type);
	GLint getUniformLocation(const std::string&);

public :
	Shader(const char* vertex_file_path, const char* fragment_file_path);
	~Shader();
	GLuint getId() const;
	void setUniformMat4(const std::string&, glm::mat4);
	void setUniformVec3(const std::string&, glm::vec3);
	void setUniform1I(const std::string&, GLint);
	void setUniform1F(const std::string&, GLfloat);
	void setUniform4F(const std::string&, GLfloat, GLfloat, GLfloat, GLfloat);
	void setUniformMat4V(const std::string& uniformName, GLuint count, GLboolean transpose, const GLfloat* value);
	void bind();
	void unbind();
	bool isBound();
};


#endif
