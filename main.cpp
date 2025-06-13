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

#include "src/graphics/models/sphere.h"
#include "src/graphics/models/cube.h"

#include <texture.h>
#include <shader.h>

using namespace std;

void processInput();
void init();

float mixVal = 0.0f;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

float x, y, z;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 200.0f));
screen screen(SCREEN_HEIGHT, SCREEN_WIDTH);
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

	Shader shader("../assets/object.vs",
		"../assets/object.fs");
	shader.activate();

	Texture texture("../assets/obama.jpg");
	if (!texture.activate(0)) {
		std::cerr << "Texture activation failed: " << std::endl;
	}

	Cube model(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, &shader, &texture);
	model.init();

	while (!screen.shouldClose()) {
		auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput();
		screen.update();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getZoom()),
			static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT),
			camera.znear, camera.zfar);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		model.render();
		screen.newFrame();
	}

	model.cleanup();
	shader.cleanup();
	texture.cleanup();
	glfwTerminate();
	return 0;
}

void processInput() {
	if (keyboard::key(GLFW_KEY_ESCAPE)) {
		screen.setShouldClose(true);
	}

	else if (keyboard::keyWentDown(GLFW_KEY_UP)) {
		mixVal += 0.25f;
		if (mixVal > 1) mixVal = 1.0f;
	}
	else if (keyboard::keyWentDown(GLFW_KEY_DOWN)) {
		mixVal -= 0.25f;
		if (mixVal < 0) mixVal = 0.0f;
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
}