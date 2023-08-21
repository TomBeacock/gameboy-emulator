#include "cpu.h"
#include "display.h"
#include "memory.h"

#include <GLFW/glfw3.h>

using namespace Gameboy;

int main(int argc, char** argv)
{
    Display display;
    Memory memory;
    CPU cpu(&memory, &display);

    glfwInit();

    GLFWwindow *window =
        glfwCreateWindow(1280, 720, "Gameboy Emulator", nullptr, nullptr);

    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
}