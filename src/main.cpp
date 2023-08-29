#include "cpu.h"
#include "display.h"
#include "mmu.h"

#include <GLFW/glfw3.h>

using namespace Gameboy;

int main(int argc, char** argv)
{
    Display display;
    MMU memory;
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