#version 330 core

layout(location = 0) in vec4 vertex_ms;
layout(location = 1) in vec3 normal_ms;
layout(location = 2) in vec2 texcoord_ms;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat4 projection_matrix;

out vec4 world_pos;

void main()
{
    world_pos = model_matrix * vertex_ms;  
    
    gl_Position = projection_matrix * view_matrix * world_pos;
}