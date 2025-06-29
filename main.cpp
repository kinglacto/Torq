#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "chunk_utility.h"
#include "src/IO/camera.h"
#include "src/IO/keyboard.h"
#include "src/IO/mouse.h"
#include "src/IO/screen.h"

// #include "src/graphics/models/sphere.h"
#include "src/graphics/models/cube.h"

#include <resource.h>

#include <assets.h>
#include <stb_image_write.h>
#include <worldgen.hpp>

#include <shader.h>
#include <texture.h>

#include <chunk_loader.h>
#include <chunk_renderer.h>
#include <filesystem>
namespace fs = std::filesystem;

void processInput();
void init();

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

float x, y, z;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 200.0f));
Screen screen(SCREEN_HEIGHT, SCREEN_WIDTH);
double mouse_dx;
double mouse_dy;
double mouse_scroll;

int main(){
	init();

	if (!screen.init()) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glfwGetCursorPos(screen.window, &mouse::lastX, &mouse::lastY);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_MULTISAMPLE);

	screen.setParameters();

	generate_data();
	ResourceManager resource{};

	resource.LoadShader(BASIC_TEXTURE_VERTEX_SHADER, BASIC_TEXTURE_FRAG_SHADER, 0);
	resource.LoadTexture(TEXTURE_DIR, 0);

	Shader* shader = resource.GetShader(0);
	shader->activate();

	Texture* texture = resource.GetTexture(0);
	if (!texture->activateAt(0)) {
		std::cerr << "Texture activation failed: " << std::endl;
	}

	auto view = glm::mat4(1.0f);
	auto projection = glm::mat4(1.0f);

	fs::path chunkDirPath(std::string(CHUNKS_DIR));
	auto chunkLoader = std::make_shared<ChunkLoader>(chunkDirPath, 3);
	
	WorldGen::setMasterSeed(123456);

	ChunkRenderer chunkRenderer{};
	chunkRenderer.chunkLoader = chunkLoader;
	chunkRenderer.texture = texture;

	while (!screen.shouldClose()) {
		auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		camera.deltaTime = deltaTime;
		processInput();
		chunkRenderer.setWorldPos(camera.cameraPos);
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getZoom()),
			static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT),
			camera.znear, camera.zfar);

		glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->activate();

		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		chunkRenderer.update();
		chunkRenderer.render(shader);
		screen.newFrame();
		double fps = 1/(double) deltaTime;
		//std::cout << fps << std::endl;
	}

	chunkRenderer.cleanup();
	resource.deleteAll();
	glfwTerminate();
	delete[] blockTexMap;
	return 0;
}

void processInput() {
	if (keyboard::key(GLFW_KEY_ESCAPE)) {
		screen.setShouldClose(true);
	}

	if (keyboard::key(GLFW_KEY_W))
		camera.updateCameraPos(cameraDirection::FORWARD, deltaTime);
	if (keyboard::key(GLFW_KEY_S))
		camera.updateCameraPos(cameraDirection::BACKWARD, deltaTime);
	if (keyboard::key(GLFW_KEY_D))
		camera.updateCameraPos(cameraDirection::RIGHT, deltaTime);
	if (keyboard::key(GLFW_KEY_A))
		camera.updateCameraPos(cameraDirection::LEFT, deltaTime);
	if (keyboard::key(GLFW_KEY_SPACE))
		camera.updateCameraPos(cameraDirection::UP, deltaTime);
	if (keyboard::key(GLFW_KEY_LEFT_SHIFT))
		camera.updateCameraPos(cameraDirection::DOWN, deltaTime);

	mouse_dx = mouse::getDX();
	mouse_dy = mouse::getDY();

	if (mouse_dx != 0 || mouse_dy != 0) {
		if (mouse::button(GLFW_MOUSE_BUTTON_LEFT)) camera.updateCameraDirection(mouse_dx, mouse_dy);
	}
}

void init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
}