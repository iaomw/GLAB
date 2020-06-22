#version 330 core
layout(location = 0) in vec4 vertex_ms;
layout(location = 1) in vec3 normal_ms;
layout(location = 2) in vec2 texcoord_ms;

out vec2 texCoords;

void main()
{
    texCoords = texcoord_ms;
	gl_Position = vertex_ms;
}