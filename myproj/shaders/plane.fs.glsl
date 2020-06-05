#version 330 core

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;

in vec4 myvertex;
in vec3 mynormal;
in vec2 mytexturecoordinate;

out vec4 color;

uniform sampler2D colortex;

void main (void)
{   	
	color = texture(colortex, mytexturecoordinate.st);
}

