#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texturecoordinate_modelspace;

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

out vec4 vertex_viewspace;
out vec3 normal_viewspace;

out vec4 myvertex;
out vec3 mynormal;
out vec2 texCoord;

void main() {
	
	myvertex = vertex_modelspace;
	mynormal = normal_modelspace;
	texCoord = texturecoordinate_modelspace;

	normal_viewspace = mynormal_matrix * mynormal.xyz;
	mat4 vmMatrix = myview_matrix * mymodel_matrix;
	vertex_viewspace = vmMatrix * vertex_modelspace;
    gl_Position = myprojection_matrix * vertex_viewspace; 
}
