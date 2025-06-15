#include "screen.h"
#include "keyboard.h"
#include "mouse.h"

int Screen::SCREEN_HEIGHT;
int Screen::SCREEN_WIDTH;

void Screen::frameBufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);

    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

Screen::Screen(int height, int width): window(nullptr) {
	SCREEN_HEIGHT = height;
	SCREEN_WIDTH = width;
};

bool Screen::init() {
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", nullptr, nullptr);
    if (!window) {
        return false;
    }

    glfwMakeContextCurrent(window);
    return true;
}

void Screen::setParameters() {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetKeyCallback(window, keyboard::keyCallback);
    glfwSetCursorPosCallback(window, mouse::cursorPosCallBack);
    glfwSetMouseButtonCallback(window, mouse::mouseButtonCallBack);
    glfwSetScrollCallback(window, mouse::mouseWheelCallBack);
	glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED)
}

void Screen::update() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::newFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Screen::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Screen::setShouldClose(const bool shouldClose) {
    glfwSetWindowShouldClose(window, shouldClose);
}
