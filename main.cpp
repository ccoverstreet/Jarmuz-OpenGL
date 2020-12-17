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
		// Check if the shader compiled succesfully. Otherwise stop execution
		int len;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char *message = (char*)alloca(len * sizeof(char));
		glGetShaderInfoLog(id, len, &len, message);

		std::cout << "Fatal Error: Unable to compile shader\n";
		std::cout << message << std::endl;

		glDeleteShader(id);

		exit(1);
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

	float positions[] = {
		-0.5, 0.5f, // 0
		-0.5f, -0.5f, // 1
		0.5f, -0.5f, // 2
		0.5f, 0.5f, // 3
		1.0, 0.5, // 4
		1.0, -0.5, // 5
		2.0, 0.0 // 6
	};


	unsigned int indices[] = {
		0, 2, 4
	};

	unsigned int indices_2[] = {
		3, 4, 5,
		2, 3, 6
	};

	ShaderProgramSource program_src = ParseShader("./shaders/basic.shader");
	unsigned int shader = CreateShader(program_src.vertex_source, program_src.fragment_source);
	glUseProgram(shader);


	VertexArray vao;
	vao.Bind();

	VertexBuffer vb(positions, sizeof(positions));
	
	vao.AddVertexBuffer(&vb);
	vao.AddVertexAttrib(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);


	vao.Unbind();

	IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));
	IndexBuffer ib_2(indices_2, sizeof(indices_2) / sizeof(unsigned int));

	std::vector<IndexBuffer*> index_vect = {&ib, &ib_2};

	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	while (!glfwWindowShouldClose(window)) 
	{
		double cur_time = glfwGetTime();
		glUseProgram(shader);

		vao.Bind(); // Bind vertex array to get state ready

		int offset_uniform = glGetUniformLocation(shader, "pos_offset");
		glUniform4f(offset_uniform, std::cos(3 * cur_time), std::sin(3 * cur_time), 0.0, std::sin(5 * cur_time) + 1);

		int rotation_uniform = glGetUniformLocation(shader, "rotation");
		float theta = 0 * cur_time;
		float rot_mat[2][2] = {
			{std::cos(theta), -1 * std::sin(theta)},
			{std::sin(theta), std::cos(theta)}
		};
		glUniformMatrix2fv(rotation_uniform, 1, GL_TRUE, (float*)rot_mat);

		int color_uniform = glGetUniformLocation(shader, "my_color");
		glUniform4f(color_uniform, 0.5 * std::sin(5 * cur_time) + 0.5, 0.5 * std::sin(10 * cur_time) + 0.5, 0.5, 1.0f);


		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, ib_2.GetCount(), GL_UNSIGNED_INT, nullptr);

		for (int i = 0; i < index_vect.size(); i++) {
			index_vect[i]->Bind();
			glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
		}


		//glDrawElements(GL_TRIANGLES, 3...) read documetation

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
