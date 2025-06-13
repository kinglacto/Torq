#ifndef SCREEN_H
#define SCREEN_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

class screen {
public:
    static int SCREEN_WIDTH;
    static int SCREEN_HEIGHT;

    static void frameBufferSizeCallback(GLFWwindow*, int width, int height);
    screen(int height, int width);
    bool init();
    void setParameters();

    void update();
    void newFrame();

    bool shouldClose();
    void setShouldClose(bool shouldClose);

    GLFWwindow* window;
};
#endif //SCREEN_H
