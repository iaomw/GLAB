#version 330 core
layout(location = 0) in vec4 vertex_modelspace;

out vec4 myvertex;
out vec3 mynormal;
out vec3 texCoord;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main()
{
    myvertex = vertex_modelspace;
    texCoord = vertex_modelspace.xyz;
    gl_Position = projection_matrix * view_matrix * model_matrix * vertex_modelspace;
}  