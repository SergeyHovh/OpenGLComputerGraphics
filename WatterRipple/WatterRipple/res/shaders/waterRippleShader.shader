#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec3 vs_color;
out vec3 vs_normal;
out vec3 vs_position;

uniform float t;
uniform mat4 mvp;
uniform mat4 model;

float PI = 3.14159265f;

float r(float x, float y, float z)
{
	return pow(position.x - x, 2) + pow(position.y - y, 2) + pow(position.z - z, 2);
}

float wave(float omega, float amp, float x, float y)
{
	float k = 2 * PI;
	float dist2 = r(x, y, 0.0f);
	float dist = sqrt(dist2);
	float time = t;
	return amp * exp(-dist) * cos(omega*time - k*dist);
}

void main()
{
	float z_coord = 0.0f
		//    omega  amp     x      y
		+ wave(5.0f, 1.0f, -0.5f, -0.5f)
		+ wave(1.0f, 1.0f, 0.5f, 0.5f)
		+ wave(1.0f, 1.0f, -0.5f, 0.5f)
		+ wave(1.0f, 1.0f, 0.5f, -0.5f)
		+ wave(5.0f, 1.0f, 1.0f, -1.0f)
		;
	z_coord /= 5;
	float col = z_coord / 2 + 0.5f;
	gl_Position = mvp * vec4(position.x, z_coord, position.y, position.w);
	vs_color = vec3(col, col, col);
	vs_normal = normal;
	vs_position = mat3(model) * position.xyz;
}

#shader fragment
#version 330 core

in vec3 vs_position;
in vec3 vs_normal;
in vec3 vs_color;

out vec4 fs_color;

uniform vec3 lightPos0;
uniform vec3 cameraPos;

void main()
{
	//Ambient light
	vec3 ambientLight = vec3(0.1f, 0.1f, 0.1f);

	//Diffuse light
	vec3 posToLightDirVec = normalize(vs_position - lightPos0);
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	float diffuse = clamp(dot(posToLightDirVec, vs_normal), 0, 1);
	vec3 diffuseFinal = diffuseColor * diffuse;

	//Specular light
	vec3 lightToPosDirVec = normalize(lightPos0 - vs_position);
	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
	vec3 posToViewDirVec = normalize(vs_position - cameraPos);
	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), 35);
	vec3 specularFinal = vec3(1.f, 1.f, 1.f) * specularConstant;

	//Final light
	fs_color = 
		vec4(vs_color, 1.f)
		* (vec4(ambientLight, 1.f) + vec4(diffuseFinal, 1.f) + vec4(specularFinal, 1.f));
}