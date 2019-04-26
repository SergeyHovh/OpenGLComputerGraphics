#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 aColor;

out vec4 vertexColor;

uniform float t;
uniform mat4 view;
float PI = 3.14159265f;

float r(float x, float y, float z)
{
	return pow(position.x - x, 2) + pow(position.y - y, 2) + pow(position.z - z, 2);
}

float wave(float omega, float amp, float x, float y, float z)
{
	float k = 2 * PI;
	float dist2 = r(x, y, z);
	float dist = sqrt(dist2);
	float time = t;
	return amp * exp(-dist2)*cos(omega*time - k*dist);
}

void main()
{
	float z_coord = 0.0f
		//    omega  amp    x      y     z
		+ wave(1.0f, 1.0f, -0.5f, -0.5f, 0.0f)
		+ wave(1.0f, 1.0f, 0.5f, 0.5f, 0.0f)
		+ wave(1.0f, 1.0f, -0.5f, 0.5f, 0.0f)
		+ wave(1.0f, 1.0f, 0.5f, -0.5f, 0.0f)
		+ wave(1.0f, 10.0f, 1.0f, 1.0f, 0.0f)
		;
	z_coord /= 5;
	float col = z_coord / 2 + 0.5f;
	gl_Position = view * vec4(position.x, z_coord, position.y, position.w);
	vertexColor = vec4(col, col, col, 1.0f);
}

#shader fragment
#version 330 core

in vec4 vertexColor;

out vec4 ourColor;
void main()
{
	ourColor = vertexColor;
}