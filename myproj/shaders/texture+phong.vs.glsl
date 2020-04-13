#version 450 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texturecoordinate_modelspace;

uniform mat4 myprojection_matrix;
uniform mat4 mymodel_matrix;
uniform mat4 myview_matrix;

out vec4 myvertex;
out vec3 mynormal;
out vec2 texCoord;

out vec4 vertex_viewspace;
out vec4 normal_viewspace;

void main() {

	vertex_viewspace = myview_matrix * mymodel_matrix * vertex_modelspace; 
    gl_Position = myprojection_matrix * vertex_viewspace; 

	texCoord = texturecoordinate_modelspace;

	myvertex = vertex_modelspace;
	mynormal = normal_modelspace;

	normal_viewspace = myview_matrix * mymodel_matrix * vec4(normal_modelspace, 1.0);
}
