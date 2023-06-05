#version 460 core

layout(location = 0) in vec4 vertex_ms;
layout(location = 1) in vec3 normal_ms;
layout(location = 2) in vec2 texcoord_ms;

uniform mat4 model_matrix;
uniform mat3 normal_matrix;

layout(std430) buffer SceneComplex
{
	mat4 projection_matrix;
	mat4 view_matrix;
	mat4 weiv_matrix;
	
	float nearZ;
	float farZ;
	float fovY;
	float XdY;

	float exposure;
	float gamma;
};

out vec2 texCoords;

void main()
{
    texCoords = texcoord_ms;
    gl_Position = model_matrix * vertex_ms;
}