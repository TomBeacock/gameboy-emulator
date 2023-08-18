cmake_minimum_required(VERSION 3.12)

project("gameboy")

add_subdirectory("external/glfw")

add_executable(gameboy
	"src/main.cpp" 
	"src/cpu.h" "src/cpu.cpp"
)
target_link_libraries(gameboy PRIVATE glfw)
target_include_directories(gameboy PRIVATE "external/glfw/include")