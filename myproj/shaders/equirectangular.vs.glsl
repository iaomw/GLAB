#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texturecoordinate_modelspace;

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;

out vec3 WorldPos;

void main()
{
    WorldPos = (mymodel_matrix * vertex_modelspace).xyz;  
    gl_Position = myprojection_matrix * myview_matrix * mymodel_matrix * vertex_modelspace;
}