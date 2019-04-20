#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 aColor;

out vec4 vertexColor;

uniform float t;
float x_offset = 0.5f;
float y_offset = -0.5f;
float x_center = position.x - x_offset;
float y_center = position.y - y_offset;
float dist2 = pow(x_center, 2) + pow(y_center, 2);
float dist = sqrt(dist2);
float PI = 3.14159265;

float sinCol(float omega)
{
	return (sin(omega*t - 2 * PI*(dist / 2 + dist2)) + 1) / dist2;
}

void main()
{
	gl_Position = position;
	float red = sinCol(PI / 3);
	float green = 0.0f;
	float blue = 0.0f;
	vertexColor = vec4(red, green, blue, 1.0f);
}

#shader fragment
#version 330 core

in vec4 vertexColor;

out vec4 ourColor;
void main()
{
	ourColor = vertexColor;
}