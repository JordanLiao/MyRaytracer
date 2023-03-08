#include "Main.h"

int main(int argc, char* argv[]) {
	Scene scene;
	scene.setup();

	int width = 900;
	int height = 600;
	Camera cam = Camera(glm::vec3(0.f, 4.f, 3.f), glm::vec3(0.0f, 0.0f, 0.f), (float)width / (float)height, glm::radians(60.f), width, height);
	TGAImage image(width, height, TGAImage::RGB);
	RTRenderer::render(&image, &scene, cam);
	//image.flip_vertically();
	image.write_tga_file("output.tga");
}