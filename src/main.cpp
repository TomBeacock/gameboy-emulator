#include <GLFW/glfw3.h>

int main(int argc, char** argv)
{
    glfwInit();

    GLFWwindow *window =
        glfwCreateWindow(1280, 720, "Gameboy Emulator", nullptr, nullptr);

    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
}