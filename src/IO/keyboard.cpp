#include "keyboard.h"

bool keyboard::keys[GLFW_KEY_LAST] = {false};
bool keyboard::keysChanged[GLFW_KEY_LAST] = {false};

void keyboard::keyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods) {
    if (action != GLFW_RELEASE) {
        if (!keys[key]) {
            keys[key] = true;
        }
    }

    else {
        keys[key] = false;
    }

    keysChanged[key] = action != GLFW_REPEAT;
}

bool keyboard::key(const int key) {
    return keys[key];
}

bool keyboard::keyChanged(const int key) {
	const bool ret = keysChanged[key];
    keysChanged[key] = false;
    return ret;
}

bool keyboard::keyWentUp(const int key) {
    return !keys[key] && keyChanged(key);
}

bool keyboard::keyWentDown(const int key) {
    return keys[key] && keyChanged(key);
}