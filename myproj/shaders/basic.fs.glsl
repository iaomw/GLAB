#version 330 core

uniform vec3 cam_position;
uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

uniform float fovY;

uniform float farZ;
uniform float nearZ;

uniform vec3 color;

in vec4 myvertex;
in vec3 mynormal;
in vec2 texCoord;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * nearZ * farZ) / (farZ + nearZ - z * (farZ - nearZ));
}

void main (void)
{   
	gExtra = myview_matrix * mymodel_matrix * myvertex; 
	vec3 cam_vspace = (myview_matrix * vec4(cam_position, 1.0)).xyz; 
	vec3 V = normalize(cam_vspace - gExtra.xyz);
    vec3 N = normalize(mynormal_matrix * mynormal);
    float NdotV = max(dot(N, V), 0.5);
	gColor = vec4(pow(color*NdotV, vec3(1.0/2.2)), 1);
	gExtra.w = LinearizeDepth(gl_FragCoord.z);
}