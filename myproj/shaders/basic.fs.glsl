#version 330 core

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

uniform float fovY;
uniform float farZ;
uniform float nearZ;
uniform float gamma;

uniform vec3 color;

in vec4 myvertex;
in vec3 mynormal;
in vec2 texcoord;

layout (location = 0) out vec4 gColor;
//layout (location = 1) out vec4 gExtra;

void main (void)
{   
	vec4 pos = view_matrix * model_matrix * myvertex; 
	vec3 V = normalize(vec3(0.0) - pos.xyz);
    vec3 N = normalize(normal_matrix * mynormal);
    float NdotV = max(dot(N, V), 0.5);
	gColor = vec4(pow(color*NdotV, vec3(1.0/gamma)), pos.z);
	//gExtra.w = LinearizeDepth(gl_FragCoord.z);
}