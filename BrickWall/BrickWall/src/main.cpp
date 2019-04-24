#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Shader
{
	const std::string vertexShader;
	const std::string fragmentShader;
};

/*reads shader from a file*/
static Shader ReadShader(const std::string filepath)
{
	enum class SHADER
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream stream(filepath);
	std::stringstream ss[2];
	std::string line;
	SHADER type = SHADER::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = SHADER::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = SHADER::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return{ ss[0].str(), ss[1].str() };
}

/*compiles the given shader*/
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	return id;
}

/*creates shader program, that consists of vertex and fragment shaders*/
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vSh = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fSh = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vSh);
	glAttachShader(program, fSh);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vSh);
	glDeleteShader(fSh);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(512, 512, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "something went wrong :(" << std::endl;

	// start


	float positions[] =
	{
		-1.0f, -1.0f, // 0
		 1.0f, -1.0f, // 1
		 1.0f,  1.0f, // 2
		-1.0f,  1.0f, // 3
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	// binding

	// bind the vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);


	// bind the index buffer
	unsigned int indexBufferObject;
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// setup layout	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);


	// shader stuff
	Shader shader = ReadShader("res/shaders/brickWallShader.shader");
	unsigned int program = CreateShader(shader.vertexShader, shader.fragmentShader);
	glUseProgram(program);

	unsigned int brick_length_location = glGetUniformLocation(program, "brick_length");
	unsigned int brick_height_location = glGetUniformLocation(program, "brick_height");
	unsigned int offset_location = glGetUniformLocation(program, "offset");

	float length = 0.2f;
	float height = 0.1f;
	float offset = 0.01f;

	// end
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glUniform1f(brick_length_location, length);
		glUniform1f(brick_height_location, height);
		glUniform1f(offset_location, offset);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}