#version 330 core

layout(location = 0) in vec4 vertex_ms;
layout(location = 1) in vec3 normal_ms;
layout(location = 2) in vec2 texcoord_ms;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

smooth out vec2 texcoord;
smooth out vec3 normal_vs;
smooth out vec4 vertex_vs;

void main() {

	normal_vs = normalize(normal_matrix * normal_ms);
	texcoord = texcoord_ms;
	
	vertex_vs = view_matrix * model_matrix * vertex_ms;
    gl_Position = projection_matrix * vertex_vs; 
}
