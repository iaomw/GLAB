#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texcoord_modelspace;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

out vec4 vertex_viewspace;
out vec3 normal_viewspace;

out vec4 myvertex;
out vec3 mynormal;
out vec2 texCoord;

void main() {
	
	myvertex = vertex_modelspace;
	mynormal = normal_modelspace;
	texCoord = texcoord_modelspace;

	normal_viewspace = normal_matrix * mynormal.xyz;
	mat4 vmMatrix = view_matrix * model_matrix;
	vertex_viewspace = vmMatrix * vertex_modelspace;
    gl_Position = projection_matrix * vertex_viewspace; 
}
