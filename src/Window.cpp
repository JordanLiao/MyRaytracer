#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "My Ray Tracer";

//projection matrix
double Window::near = 0.1;
double Window::far = 1000.0;
double Window::fov = 60.0;
glm::mat4 Window::projection = glm::mat4(1); // initially set to the identity matrix because window size is unknown, resizecallback()
                                             // would give the projection matrix an actual value.


GLFWwindow* Window::createWindow(int initWidth, int initHeight, float fov) {
	Window::fov = fov;

	// Initialize GLFW.
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window.
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int xpos, ypos, workAreawidth, workAreaheight;
	glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &workAreawidth, &workAreaheight);

	if (initWidth < 0 || initHeight < 0) {
		std::cerr << "Incorrect Window Dimension" << std::endl;
		return NULL;
	}

	GLFWwindow* window = glfwCreateWindow(initWidth, initHeight, windowTitle, NULL, NULL);
	glfwSetWindowPos(window, xpos, ypos + 30);
	if (!window) {
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	//set up opengl using glew
	if (glewInit()) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}

	// Set swap interval to 1 to have vSync.
	glfwSwapInterval(1);

	Window::resizeCallback(window, initWidth, initHeight);

	return window;
}

//updating framebuffers if the screen size changes
void Window::resizeCallback(GLFWwindow* window, int width, int height) {
	//if window is minimized, do nothing to the projection matrix, else the app would crash.
	if (width <= 0 || height <= 0)
		return;

	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glcheck(glViewport(0, 0, width, height));
	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(fov),
		double(width) / (double)height, near, far);
}



