#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 coordinate_modelspace;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

smooth out vec2 coordinate;
smooth out vec4 vertex_viewspace;
smooth out vec3 normal_viewspace;

void main() {
	
	coordinate = coordinate_modelspace;
	normal_viewspace = normalize(normal_matrix * normal_modelspace);

	vertex_viewspace = view_matrix * model_matrix * vertex_modelspace;
    gl_Position = projection_matrix * vertex_viewspace; 
}
