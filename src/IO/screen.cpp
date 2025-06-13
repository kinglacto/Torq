#include "screen.h"
#include "keyboard.h"
#include "mouse.h"

int screen::SCREEN_HEIGHT;
int screen::SCREEN_WIDTH;

void screen::frameBufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);

    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

screen::screen(int height, int width): window(nullptr) {
	SCREEN_HEIGHT = height;
	SCREEN_WIDTH = width;
};

bool screen::init() {
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", nullptr, nullptr);
    if (!window) {
        return false;
    }

    glfwMakeContextCurrent(window);
    return true;
}

void screen::setParameters() {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetKeyCallback(window, keyboard::keyCallback);
    glfwSetCursorPosCallback(window, mouse::cursorPosCallBack);
    glfwSetMouseButtonCallback(window, mouse::mouseButtonCallBack);
    glfwSetScrollCallback(window, mouse::mouseWheelCallBack);
	glEnable(GL_DEPTH_TEST);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED)
}

void screen::update() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void screen::newFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool screen::shouldClose() {
    return glfwWindowShouldClose(window);
}

void screen::setShouldClose(const bool shouldClose) {
    glfwSetWindowShouldClose(window, shouldClose);
}
