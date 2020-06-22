#version 460 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) in vec4 vertex_ms;
layout(location = 1) in vec3 normal_ms;
layout(location = 2) in vec2 texcoord_ms;

layout(location = 3) in uint drawid;
layout(location = 4) in mat4 model_matrix;
layout(location = 8) in mat4 normal_matrix;

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

smooth out vec2 texcoord;
smooth out vec3 normal_vs;
smooth out vec4 vertex_vs;

void main() {

	normal_vs = normalize(normal_matrix * vec4(normal_ms, 1.0)).rgb;
	texcoord = texcoord_ms;
	
	vertex_vs = view_matrix * model_matrix * vertex_ms;
    gl_Position = projection_matrix * vertex_vs; 
}
