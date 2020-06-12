#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texcoord_modelspace;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

out vec2 texCoords;

void main()
{
    texCoords = texcoord_modelspace;
    gl_Position = model_matrix * vertex_modelspace;
}