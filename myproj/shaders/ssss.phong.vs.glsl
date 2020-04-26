#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 coordinate_modelspace;

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

out vec2 coordinate;
out vec4 vertex_viewspace;
out vec3 normal_viewspace;

void main() {
	
	coordinate = coordinate_modelspace;
	normal_viewspace = normalize(mynormal_matrix * normal_modelspace);

	vertex_viewspace = myview_matrix * mymodel_matrix * vertex_modelspace;
    gl_Position = myprojection_matrix * vertex_viewspace; 
}
