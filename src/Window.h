#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Core.h"

enum class framebuffer {
	defaultFrame, //GLFW managed framebuffer
	pickingFrame, //application managed framebuffer for model picking
	shadowMapFrame
};

class Window {
public:

	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;

	static double near, far, fov;
	static glm::mat4 projection;

	// Window functions
	static GLFWwindow* createWindow(int initWidth, int initHeight);
	static void resizeCallback(GLFWwindow* window, int width, int height);
};

#endif