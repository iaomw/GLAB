#version 330 core

uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 mymodel_matrix;

in vec4 myvertex;
in vec3 mynormal;
in vec2 mytexturecoordinate;

out vec4 color;

uniform sampler2D tex;

uniform int totexture;

uniform struct Light 
{
	vec4 position;
	vec4 intensity;
	vec3 direction;
	int type;
} Lights[32];
uniform int num_lights;

uniform struct Material
{
	vec4 kd;
	vec4 ks;
	vec4 ka;
	float specular;
} material;


void main (void)
{   
    if (totexture == 1) 
		color =  texture(tex, mytexturecoordinate.st);
	else if (totexture == 0)
		color = material.kd;

	color.a = 1.0f;
}

