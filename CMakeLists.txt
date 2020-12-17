cmake_minimum_required(VERSION 3.10)

# set the project name
project(JarmuzOpenGL)

add_executable(jarmuzopengl main.cpp)

set_target_properties(jarmuzopengl
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
	)

add_subdirectory(src)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

target_link_libraries(jarmuzopengl -lGL -lGLEW -lglfw -lm)

