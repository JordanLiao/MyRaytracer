#include "Main.h"

void rayTest(Scene* scene) {
	glm::vec3 o(0.f, 0.5f, 0.1f);
	glm::vec3 r = glm::normalize(glm::vec3(0.3f, 0.f, -0.8f));
	Intersect intersect;
	if (RTRenderer::trace(intersect, o, r, scene->objects)) {
		RTRenderer::shade(r, intersect, scene);
	}
}

int main(int argc, char* argv[]) {
	srand(time(nullptr));
	Scene scene;
	scene.setup();
	
#if _DEBUG
	int width = 3;
	int height = 3;
#else 
	int width = 900;
	int height = 600;
#endif

	rayTest(&scene);

	//Camera cam = Camera(glm::vec3(0.f, 0.3f, 0.1f), glm::vec3(0.0f, 0.3f, 0.f), (float)width / (float)height, glm::radians(60.f), width, height);
	Camera cam = Camera(glm::vec3(0.f, 1.5f, 4.f), glm::vec3(0.0f, 1.5f, 0.f), (float)width / (float)height, glm::radians(60.f), width, height);
	TGAImage image(width, height, TGAImage::RGB);
	RTRenderer::render(image, &scene, cam);
	image.write_tga_file("output.tga"); 
	
}