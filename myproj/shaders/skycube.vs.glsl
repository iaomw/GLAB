#version 330 core
layout(location = 0) in vec4 vertex_modelspace;

out vec3 texCoords;

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;

void main()
{
    texCoords = vertex_modelspace.xyz;
    gl_Position = myprojection_matrix * myview_matrix * mymodel_matrix * vertex_modelspace;
}  