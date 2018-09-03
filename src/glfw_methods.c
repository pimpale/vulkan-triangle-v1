#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <GLFW/glfw3.h>

#include "constants.h"
#include "glfw_methods.h"

GLFWwindow* createGlfwWindow()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    return glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", NULL, NULL);
}

