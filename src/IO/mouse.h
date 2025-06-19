#pragma once

#include <GLFW/glfw3.h>

class mouse {
public:
    static void cursorPosCallBack(GLFWwindow* window, double _x, double _y);
    static void mouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);
    static void mouseWheelCallBack(GLFWwindow* window, double dx, double dy);

    static double getMouseX();
    static double getMouseY();

    static double getDX();
    static double getDY();

    static double getScrollDX();
    static double getScrollDY();

    static bool button(int button);
    static bool buttonChanged(int button);
    static bool buttonWentUp(int button);
    static bool buttonWentDown(int button);

    static double lastX;
    static double lastY;

private:
    static double x;
    static double y;

    static double dx;
    static double dy;

    static double scrollDX;
    static double scrollDY;

    static bool buttons[];
    static bool buttonsChanged[];
};