#version 330 core
layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texturecoordinate_modelspace;

out vec2 texCoords;

void main()
{
    texCoords = texturecoordinate_modelspace;
	gl_Position = vertex_modelspace;
}