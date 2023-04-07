#include "Main.h"

int main(int argc, char* argv[]) {
	Scene scene;
	scene.setup();
	
#if _DEBUG
	int width = 100;
	int height = 100;
#else 
	int width = 900;
	int height = 600;
#endif
	//Camera cam = Camera(glm::vec3(0.f, 1.5f, 4.5f), glm::vec3(0.0f, 1.5f, 0.f), (float)width / (float)height, glm::radians(60.f), width, height);
	Camera cam = Camera(glm::vec3(0.f, 1.5f, 4.f), glm::vec3(0.0f, 1.5f, 0.f), (float)width / (float)height, glm::radians(60.f), width, height);
	TGAImage image(width, height, TGAImage::RGB);
	RTRenderer::render(image, &scene, cam);
	image.write_tga_file("output.tga");
}