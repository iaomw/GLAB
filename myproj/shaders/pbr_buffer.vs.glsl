#version 460 core

layout(location = 0) in vec4 vertex_ms;
layout(location = 1) in vec3 normal_ms;
layout(location = 2) in vec2 texcoord_ms;

layout(location = 3) in uint drawid;
layout(location = 4) in mat4 model_matrix;
layout(location = 8) in mat4 normal_matrix;

out vec2 texcoord;

void main() {
	texcoord = texcoord_ms;
    gl_Position = vertex_ms; 
}