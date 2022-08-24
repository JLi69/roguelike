#ifndef INCLUDE_EVENT
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//Key callback function
void onKeyPress(GLFWwindow *win, int key, int scancode, int action, int mods);
//Check if a key is pressed
//Returns 0 if no,
//Returns 1 if yes
//Can detect up to 4 keys being pressed
int checkKey(int key);

//Handle window resizing
void handleWinResize(GLFWwindow *win, int newWidth, int newHeight);

#endif
