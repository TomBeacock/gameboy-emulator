cmake_minimum_required(VERSION 3.12)

project("gameboy")

add_subdirectory("external/glfw")


add_executable(gameboy
	"src/main.cpp" 
	"src/types.h"
	"src/cpu.h" "src/cpu.cpp"
	"src/mmu.h" "src/mmu.cpp"
	"src/ppu.h" "src/ppu.cpp"
	"src/display.h"
)
set_target_properties(gameboy PROPERTIES CXX_STANDARD 20)
target_link_libraries(gameboy PRIVATE glfw)
target_include_directories(gameboy PRIVATE "external/glfw/include")