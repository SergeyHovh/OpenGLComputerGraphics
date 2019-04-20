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

void generate_plane(const int cols, const int rows, float* positions, unsigned int* indices)
{
	float x_step = 2.0 / (cols - 1);
	float y_step = 2.0 / (rows - 1);
	float x = -1.0f, y = 1.0f;
	const float z = 0.0f;
	// float r = 1.0f, g = 1.0f, b = 0.0f;
	int count = 0;
	// positions
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if (abs(x) < 1.0e-3) x = 0.0f;
			if (abs(y) < 1.0e-3) y = 0.0f;
			positions[count + 0] = x;
			positions[count + 1] = y;
			positions[count + 2] = z;
			positions[count + 3] = 0.0f;
			positions[count + 4] = (z + 1.0) / 2;
			positions[count + 5] = 0.0f;
			x += x_step;
			count += 6;
		}
		y -= y_step;
		x = -1.0f;
	}
	// index generation
	const auto size = 2 * cols * (rows - 1) + 2 * (rows - 2);
	auto* index = new unsigned int[size];
	count = 0;
	for (int i = 0; i < rows - 1; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			auto k = j + i*cols;
			index[count] = k;
			count++;
			index[count] = k + cols;
			count++;
		}
		if (i != rows - 2)
		{
			index[count] = (i + 2)*cols - 1;
			count++;
			index[count] = (i + 1)*cols;
			count++;
		}
	}
	// indices
	int k = 0;
	for (int i = 0; i < size - 2; ++i)
	{
		indices[k] = index[i];
		k++;
		indices[k] = index[i + 1];
		k++;
		indices[k] = index[i + 2];
		k++;
	}
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

	// start
	const auto a = 2 << 8;
	const auto cols = a, rows = a;
	const auto size = 2 * cols * (rows - 1) + 2 * (rows - 2);
	const auto number_of_attribs = 6;
	const auto number_of_vertices = number_of_attribs * cols*rows;
	const auto number_of_indices = 3 * (size - 2);
	auto* pos = new float[number_of_vertices];
	auto* ind = new unsigned int[number_of_indices];

	generate_plane(cols, rows, pos, ind);

	// binding

	// bind the vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, number_of_vertices * sizeof(float), pos, GL_STATIC_DRAW);


	// bind the index buffer
	unsigned int indexBufferObject;
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_of_indices * sizeof(unsigned int), ind, GL_STATIC_DRAW);

	// setup layout	

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, number_of_attribs * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, number_of_attribs * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// shader stuff
	Shader shader = ReadShader("res/shaders/waterRippleShader.shader");
	unsigned int program = CreateShader(shader.vertexShader, shader.fragmentShader);
	glUseProgram(program);

	unsigned int tLocation = glGetUniformLocation(program, "t");
	float t = 0.0f;
	// end
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUniform1f(tLocation, t);
		
		glDrawElements(GL_TRIANGLE_STRIP, number_of_indices, GL_UNSIGNED_INT, nullptr);
		
		t += 0.05f;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}