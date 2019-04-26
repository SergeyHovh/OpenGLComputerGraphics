#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
void main()
{
	gl_Position = position;
}

#shader fragment
#version 330 core

out vec4 color;

uniform float brick_length;
uniform float brick_height;
uniform float offset;

vec4 border_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

float x = gl_FragCoord.x / 512;
float y = gl_FragCoord.y / 512;
float L = brick_length + offset;
float H = brick_height + offset;

void main()
{
	int i = int(x / L);
	int j = int(y / H);
	float col = sin(i + j) / 6 + 0.5f;
	if (mod(x, L) < offset || mod(y, H) < offset) color = border_color;
	else color = vec4(col, 0.0f, 0.0f, 1.0f);
}