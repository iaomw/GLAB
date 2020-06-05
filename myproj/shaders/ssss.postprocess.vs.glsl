#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texcoord_modelspace;

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

out vec4 myvertex;
out vec3 mynormal;
out vec2 mytexturecoordinate;

void main() {
    gl_Position = vertex_modelspace; 
	myvertex = vertex_modelspace;
	mynormal = normal_modelspace;
	mytexturecoordinate = texcoord_modelspace;
}
