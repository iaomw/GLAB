#version 460 core

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

out flat uint drawID;

out vec4 vertex_vs;
out vec3 normal_vs;
out vec2 texcoord;

void main() {

	drawID = drawid;
	//drawID = gl_DrawID;
	//drawID = gl_InstanceID;

	normal_vs = normalize(normal_matrix * vec4(normal_ms, 0.0)).xyz;
	vertex_vs = view_matrix * model_matrix * vertex_ms;
	texcoord = texcoord_ms;

    gl_Position = projection_matrix * vertex_vs; 
}
