#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class keyboard {
public:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static bool key(int key);
    static bool keyChanged(int key);
    static bool keyWentUp(int key);
    static bool keyWentDown(int key);

private:
    static bool keys[];
    static bool keysChanged[];
};

