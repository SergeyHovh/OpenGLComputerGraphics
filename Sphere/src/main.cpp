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

static unsigned int* stripIndices(unsigned int* indices, const int length, const int rows)
{
	const auto size = 2 * length * (rows - 1) + 2 * (rows - 2);
	auto* result = new unsigned int[(size - 2) * 3];
	auto c = 0;
	for (int i = 0; i < size - 2; ++i)
	{
		result[c] = indices[i];
		c++;
		result[c] = indices[i + 1];
		c++;
		result[c] = indices[i + 2];
		c++;
	}
	return result;
}

static unsigned int* indices(const int length, const int rows)
{
	const auto size = 2 * length * (rows - 1) + 2 * (rows - 2);
	auto* index = new unsigned int[size];
	auto c = 0;
	for (int i = 0; i < rows - 1; ++i)
	{
		for (int j = 0; j < length; ++j)
		{
			auto k = j + i * length + 1;
			index[c] = k;
			c++;
			index[c] = k + length;
			c++;
		}
		if (i != rows - 2)
		{
			index[c] = (i + 2) * length;
			c++;
			index[c] = (i + 1) * length + 1;
			c++;
		}
	}
	return index;
}

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
	const auto length = 5;
	const auto rows = 3;
	const auto size = 2 * length * (rows - 1) + 2 * (rows - 2);
	const auto stripSize = (size - 2) * 3;
	auto* strip_indices = stripIndices(indices(length, rows), length, rows);
	for (int i = 0; i < stripSize; ++i)
	{
		std::cout << strip_indices[i] << std::endl;
	}

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(500, 500, "Hello World", NULL, NULL);
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

	float positions[]
	{
		-0.5f, -0.5f, // 0
		 0.5f, -0.5f, // 1
		 0.5f,  0.5f, // 2
		-0.5f,  0.5f, // 3
		 0.0f,  0.8f  // 4
	};

	unsigned int indices[]
	{
		0, 1, 2,
		2, 3, 0,
		2, 4, 3
	};

	const int numberOfIndices = 3 * 3;
	const int numberOfVertices = 5;
	const int numberOfAttributes = 2;
	const int sizeOfTheArray = numberOfAttributes * numberOfVertices;

	// bind the vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeOfTheArray * sizeof(float), positions, GL_STATIC_DRAW);

	// bind the index buffer
	unsigned int indexBufferObject;
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// setup layout
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// shader
	Shader shader = ReadShader("res/shaders/basicShader.shader");
	unsigned int program = CreateShader(shader.vertexShader, shader.fragmentShader);
	glUseProgram(program);

	int location = glGetUniformLocation(program, "u_Color");
	float r = 0.0, g = 0.5, b = 0.8, incR = 0.01, incG = incR, incB = incR;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		_ASSERT(location != 1);
		glUniform4f(location, r, g, b, 1.0);

		r += 3 * incR;
		g += 2 * incG;
		b += 7 * incB;
		if (r > 1.0 || r < 0.0) incR *= -1;
		if (g > 1.0 || g < 0.0) incG *= -1;
		if (b > 1.0 || b < 0.0) incB *= -1;


		glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}