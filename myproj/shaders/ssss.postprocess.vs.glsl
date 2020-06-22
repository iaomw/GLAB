#version 330 core

layout(location = 0) in vec4 vertex_ms;
layout(location = 1) in vec3 normal_ms;
layout(location = 2) in vec2 texcoord_ms;

out vec4 myvertex;
out vec3 mynormal;
out vec2 mytexturecoordinate;

void main() {
    gl_Position = vertex_ms; 
	myvertex = vertex_ms;
	mynormal = normal_ms;
	mytexturecoordinate = texcoord_ms;
}
