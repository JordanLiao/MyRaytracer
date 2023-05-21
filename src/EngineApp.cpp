#include "EngineApp.h"

GLFWwindow* EngineApp::window;


//mouse/keyboard states
bool EngineApp::mousePressed[3];
cursorState EngineApp::currentCursorState = cursorState::idle;
double EngineApp::cursorPosition[2] = {0.0, 0.0};
double EngineApp::prevCursorPosition[2] = {0.0, 0.0};
bool EngineApp::keyPressed[350];

uint64_t EngineApp::appStartTime, EngineApp::appCurrTime, EngineApp::appDuration;

/************************************************************************************************
	NOTE!!:
		Static objects need to have heap allocation using pointer because not every dependency might
	be initialized at the beginning if the program were to left to init them on its own.
		Declare pointers down here, and init them in the initializeObject() function.
*/

/************************************************************************************************/

bool EngineApp::initializeProgram(GLFWwindow* w) {
	window = w;

	appStartTime = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	appCurrTime = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	appDuration = appCurrTime - appStartTime;

	return true;
}

bool EngineApp::initializeObjects() {
	//shadowTest = new ShadowTester();

	return true;
}

void EngineApp::idleCallback() {
	appCurrTime = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	appDuration = appCurrTime - appStartTime;
	// Perform any necessary updates here
	if (currentCursorState == cursorState::freeCam) { //move camera in free cam
		if (keyPressed[GLFW_KEY_W])
			moveCamera(GLFW_KEY_W);
		if (keyPressed[GLFW_KEY_A])
			moveCamera(GLFW_KEY_A);
		if (keyPressed[GLFW_KEY_S])
			moveCamera(GLFW_KEY_S);
		if (keyPressed[GLFW_KEY_D])
			moveCamera(GLFW_KEY_D);
		if (keyPressed[GLFW_KEY_LEFT_SHIFT])
			moveCamera(GLFW_KEY_LEFT_SHIFT);
		if (keyPressed[GLFW_KEY_LEFT_CONTROL])
			moveCamera(GLFW_KEY_LEFT_CONTROL);
	}
}

void EngineApp::displayCallback() {
}


void EngineApp::moveCamera(int key) {
	if (key == GLFW_KEY_W)
		return;
	else if (key == GLFW_KEY_A)
		return;
	else if (key == GLFW_KEY_S)
		return;
	else if (key == GLFW_KEY_D)
		return;
	else if (key == GLFW_KEY_LEFT_SHIFT)
		return;
	else if (key == GLFW_KEY_LEFT_CONTROL)
		return;
}

void EngineApp::cleanUp() {

}

void EngineApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		keyPressed[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		keyPressed[key] = false;
	}
}

void EngineApp::cursor_callback(GLFWwindow* window, double currX, double currY) {
	double dx = currX - cursorPosition[0];
	double dy = currY - cursorPosition[1];

	cursorPosition[0] = currX;
	cursorPosition[1] = currY;
}

void EngineApp::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) { // if left mouse button pressed
		if(button == GLFW_MOUSE_BUTTON_LEFT) { // if left mouse button pressed
			return;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) { // if right mouse button pressed
			return;
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {  // scroll wheel pressed
			return;
		}
	}

	//release actions
	if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			return;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			return;
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			return;
		}
	}
}

