#version 330 core

uniform sampler2D colortex;

in vec2 texcoord;

out vec4 color;

void main (void)
{   	
	color = texture(colortex, texcoord.st);
}

