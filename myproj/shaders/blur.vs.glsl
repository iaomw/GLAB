#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texturecoordinate_modelspace;

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

out vec2 TexCoords;

void main()
{
    TexCoords = texturecoordinate_modelspace;
    gl_Position = mymodel_matrix * vertex_modelspace;
}