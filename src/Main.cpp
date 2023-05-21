#include "RTRenderer.h"
#include "Scene.h"
#include "RayTracer.h"
#include "Window.h"
#include "EngineApp.h"
#include "Shader.h"
#include "Tests.h"

void error_callback(int error, const char* description)
{
	std::cerr << description << std::endl;
}

void setup_callbacks(GLFWwindow* window)
{
	// Set the error callback.
	glfwSetErrorCallback(error_callback);
	// Set the window resize callback.
	glfwSetWindowSizeCallback(window, Window::resizeCallback);

	// Set the key callback.
	glfwSetKeyCallback(window, EngineApp::keyCallback);

	// Set the mouse and cursor callbacks
	glfwSetMouseButtonCallback(window, EngineApp::mouse_callback);
	//glfwSetScrollCallback(window, EngineApp::scroll_callback);
	glfwSetCursorPosCallback(window, EngineApp::cursor_callback);
}

void setup_opengl_settings() {
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glClearColor(0.0, 0.0, 0.0, 0.0);
}

void print_versions() {
	// Get info of GPU and supported OpenGL version.
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported: " << glGetString(GL_VERSION) << " \n" << std::endl;
}

int main(int argc, char* argv[]) {
	srand(time(nullptr));
	
	float fov = 50.f;
#if _DEBUG
	int width = 3;
	int height = 3;
#else 
	int width = 600;
	int height = 400;
#endif
	
	//RTCamera cam = RTCamera(glm::vec3(0.f, 1.5f, 4.f), glm::vec3(0.0f, 1.5f, 0.f), (float)width / (float)height, glm::radians(fov), width, height);
	RTCamera cam = RTCamera(glm::vec3(0.f, 0.f, 4.f), glm::vec3(0.0f, 0.0f, 0.f), glm::vec3(0.f, 1.f, 0.f), (float)width / (float)height, glm::radians(fov), width, height);
	
	GLFWwindow* window = Window::createWindow(width, height, fov);
	if (!window) 
		exit(EXIT_FAILURE);
	print_versions();
	setup_callbacks(window);
	setup_opengl_settings();

	Shader* simpleShader = new Shader("src/shaders/shader.vert", "src/shaders/shader.frag");
	GLuint shaderID = simpleShader->getId();

	Scene scene;
	scene.setup();

	//intersectTest(&scene);

	
	TGAImage image(width, height, TGAImage::RGB);
	RTRenderer::render(image, &scene, cam);
	image.write_tga_file("output.tga");
	

	glm::mat4 projView = Window::projection * cam.view;
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//updating app state
		for (const Object* obj : scene.objects) {
			if(obj->mdf)
				obj->mdf->drawMDFBoundary(shaderID, glm::vec3(1.f), projView);
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	EngineApp::cleanUp();
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	
}