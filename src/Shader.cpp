#include "shader.h"

Shader* Shader::currBoundShader = nullptr;

Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath) {
	//pre assign the shader program id to be 0 for later error checking
	//it should not be zero if shader is compiled and attached correctly
	programID = 0;

	// Create the vertex shader and fragment shader.
	GLuint vertexShaderID = LoadSingleShader(vertexFilePath, shaderType::vertex);
	GLuint fragmentShaderID = LoadSingleShader(fragmentFilePath, shaderType::fragment);

	// Check both shaders.
	if (vertexShaderID == 0 || fragmentShaderID == 0) return;

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Link the program.
	std::cout << "Linking program" << std::endl;
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check the program.
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage.data());
		std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
		std::cerr << msg << std::endl;
		glDeleteProgram(programID);
		return;
	}
	else
	{
		std::cout << "Successfully linked program!\n" << std::endl;;
	}

	// Detach and delete the shaders as they are no longer needed.
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

Shader::~Shader()
{
	if(programID > 0)
		glDeleteProgram(programID);
}

GLuint Shader::LoadSingleShader(const char* shaderFilePath, shaderType type)
{
	// Create a shader id.
	GLuint shaderID = 0;
	if (type == shaderType::vertex)
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	else if (type == shaderType::fragment)
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Try to read shader codes from the shader file.
	std::string shaderCode;
	std::ifstream shaderStream(shaderFilePath, std::ios::in);
	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else
	{
		std::cerr << "Impossible to open " << shaderFilePath << ". "
			<< "Check to make sure the file exists and you passed in the "
			<< "right filepath!"
			<< std::endl;
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader.
	std::cerr << "Compiling shader: " << shaderFilePath << std::endl;
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// Check Shader.
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage.data());
		std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
		std::cerr << msg << std::endl;
		return 0;
	}
	else
	{
		if (type == shaderType::vertex)
			std::cout << "Successfully compiled vertex shader!" << std::endl;
		else if (type == shaderType::fragment)
			std::cout << "Successfully compiled fragment shader!" << std::endl;
	}

	return shaderID;
}

/*
	get the uniform location from cache uniformMap,
	or find the location and cache it in uniformMap.
*/

GLint Shader::getUniformLocation(const std::string& uName)
{
	if (uniformMap.find(uName) != uniformMap.end()) {
		return uniformMap[uName];
	}
	GLint location = glGetUniformLocation(programID, uName.c_str());
	if (location == -1) // if location cannot be found within the shader program, return -1
		return location;
	uniformMap[uName] = location;
	return location;
}

GLuint Shader::getId() const{
	return programID;
}

void Shader::setUniformMat4(const std::string& uName, glm::mat4 uValue){
	glcheck(glUniformMatrix4fv(getUniformLocation(uName), 1, false, glm::value_ptr(uValue)));
}

void Shader::setUniformVec3(const std::string& uName, glm::vec3 uValue){
	glcheck(glUniform3fv(getUniformLocation(uName), 1, glm::value_ptr(uValue)));
}

void Shader::setUniform1I(const std::string& uName, GLint uValue){
	glcheck(glUniform1i(getUniformLocation(uName), uValue));
}

void Shader::setUniform1F(const std::string& uName, GLfloat uValue){
	glcheck(glUniform1f(getUniformLocation(uName), uValue));
}

void  Shader::setUniform4F(const std::string& uName, GLfloat uValue0, GLfloat uValue1, GLfloat uValue2, GLfloat uValue3){
	glcheck(glUniform4f(getUniformLocation(uName), uValue0, uValue1, uValue2, uValue3));
}

void  Shader::setUniformMat4V(const std::string& uniformName, GLuint count, GLboolean transpose, const GLfloat* value){
	glcheck(glUniformMatrix4fv(getUniformLocation(uniformName), count, transpose, value));
}

void Shader::bind() {
	if (this == currBoundShader) // if already bound, do not bound again.
		return;
	glcheck(glUseProgram(programID));
	currBoundShader = this;
}

void Shader::unbind() {
	glcheck(glUseProgram(0));
	currBoundShader = nullptr;
}

bool Shader::isBound() {
	return this == currBoundShader;
}
