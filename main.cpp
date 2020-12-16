#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>

#include "src/vertex_buffer.h"
#include "src/index_buffer.h"
#include "src/vertex_array.h"


void error_callback(int error, const char *description) {
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void GLErrorCallback(GLenum source,
	 GLenum type,
	 GLuint id,
	 GLenum severity,
	 GLsizei length,
	 const GLchar* message,
	 const void* userParam )
{
	fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    	( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
        type, severity, message );
}

struct ShaderProgramSource
{
	std::string vertex_source;
	std::string fragment_source;
};

static ShaderProgramSource ParseShader(const std::string &file_path) 
{
	std::ifstream shader_file(file_path);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(shader_file, line)) 
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// ADD ERROR HANDLING
	// Shader may have syntax errors.
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) 
	{
		int len;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char *message = (char*)alloca(len * sizeof(char));
		glGetShaderInfoLog(id, len, &len, message);

		std::cout << "Failed to compile shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
	}

	return id;
}

static unsigned int CreateShader(const std::string vertexShader, const std::string &fragmentShader) 
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main() {
	GLFWwindow* window;

	if (!glfwInit()) {
		return 1;
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(640, 480, "Simple Example", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return 1;
	}


	glfwMakeContextCurrent(window);
	std::cout << glGetString(GL_VERSION) << std::endl;

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) 
	{
		std::cout << "Error: Unable to intialize GLEW\n";
	}


	glfwSetKeyCallback(window, key_callback);
	
	glEnable (GL_DEBUG_OUTPUT);
	glDebugMessageCallback (GLErrorCallback, 0);

	float positions[12] = {
		-0.5, 0.5f, // 0
		-0.5f, -0.5f, // 1
		0.5f, -0.5f, // 2
		0.5f, 0.5f // 3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	ShaderProgramSource program_src = ParseShader("./shaders/basic.shader");
	unsigned int shader = CreateShader(program_src.vertex_source, program_src.fragment_source);
	glUseProgram(shader);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	VertexBuffer vb(positions, 4 * 2 * sizeof(float));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	IndexBuffer ib(indices, 6);

	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	while (!glfwWindowShouldClose(window)) 
	{
		double cur_time = glfwGetTime();
		glUseProgram(shader);
		glBindVertexArray(vao);

		ib.Bind();

		int offset_uniform = glGetUniformLocation(shader, "pos_offset");
		glUniform4f(offset_uniform, std::cos(3 * cur_time), std::sin(3 * cur_time), 0.0, 1);

		int rotation_uniform = glGetUniformLocation(shader, "rotation");
		float theta = 10 * cur_time;
		float rot_mat[2][2] = {
			{std::cos(theta), -1 * std::sin(theta)},
			{std::sin(theta), std::cos(theta)}
		};
		glUniformMatrix2fv(rotation_uniform, 1, GL_TRUE, (float*)rot_mat);

		int color_uniform = glGetUniformLocation(shader, "my_color");
		glUniform4f(color_uniform, 0.5 * std::sin(9 * cur_time) + 0.5, 0.5 * std::sin(10 * cur_time) + 0.5, 0.0f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		// glDrawElements(GL_TRIANGLES, 3...) read documetation

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
