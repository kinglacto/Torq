#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>

class Screen {
public:
    static int SCREEN_WIDTH;
    static int SCREEN_HEIGHT;

    static void frameBufferSizeCallback(GLFWwindow*, int width, int height);
    Screen(int height, int width);
    bool init();
    void setParameters();

    void update();
    void newFrame();

    bool shouldClose();
    void setShouldClose(bool shouldClose);

    GLFWwindow* window;
};
