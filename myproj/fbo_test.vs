#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texturecoordinate_modelspace;

uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 mymodel_matrix;

in vec4 myvertex;
in vec3 mynormal;
in vec2 mytexturecoordinate;

out vec4 color;

uniform sampler2D tex;
uniform int totexture;

void main() {
    gl_Position = myprojection_matrix * myview_matrix * mymodel_matrix * vertex_modelspace; 
	myvertex = vertex_modelspace;
	mynormal = normal_modelspace;
	mytexturecoordinate = texturecoordinate_modelspace;
}
