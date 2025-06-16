#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/IO/camera.h"
#include "src/IO/keyboard.h"
#include "src/IO/mouse.h"
#include "src/IO/screen.h"

#include "src/graphics/models/cube.h"

#include <resource.h>

#include "assets.h"

#include <shader.h>
#include <texture.h>

using namespace std;

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

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glfwGetCursorPos(screen.window, &mouse::lastX, &mouse::lastY);
	screen.setParameters();

	ResourceManager resource{};

	resource.LoadShader(BASIC_TEXTURE_VERTEX_SHADER, BASIC_TEXTURE_FRAG_SHADER, 0);
	resource.LoadTexture(TEXTURE_DIR, 0);

	Shader* shader = resource.GetShader(0);
	shader->activate();

	Texture* texture = resource.GetTexture(0);
	if (!texture->activate(0)) {
		std::cerr << "Texture activation failed: " << std::endl;
	}

	Cube model(glm::vec3(0.0f, 0.0f, 0.0f), 16.0f, shader, texture);
	model.init();

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);


	while (!screen.shouldClose()) {
		auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		camera.deltaTime = deltaTime;
		processInput();
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getZoom()),
			static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT),
			camera.znear, camera.zfar);

		screen.update();

		shader->activate();

		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		model.render();
		screen.newFrame();
	}

	model.cleanup();
	resource.deleteAll();
	glfwTerminate();
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

	mouse_scroll = mouse::getScrollDY();
	if (mouse_scroll != 0) {
		camera.updateCameraZoom(mouse_scroll);
	}
}

void init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 8);
}