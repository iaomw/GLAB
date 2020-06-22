#version 460 core

layout(location = 0) out vec4 gColor;

layout(std430) buffer SceneComplex
{
	mat4 projection_matrix;
	mat4 view_matrix;
	mat4 weiv_matrix;
	
	float nearZ;
	float farZ;
	float fovY;
	float XdY;

	float exposure;
	float gamma;
};

struct Light {
    uvec2 type; 
	uvec2 shadow_handle; 

    vec4 color;
    vec4 position;
    vec4 intensity;	
    vec4 direction;
	mat4 model_matrix;
};

layout(std430) buffer Light_Pack
{
	uint lightCount;
	Light lightList[];
};

in flat uint drawID;

in vec4 vertex_vs;
in vec3 normal_vs;
in vec2 texcoord;

void main (void)
{   
	vec3 color = lightList[drawID].color.rgb;

	vec3 N = normal_vs;
	vec3 V = normalize(-vertex_vs.xyz);
    
    float NdotV = max(dot(N, V), 0.5);
	gColor = vec4(pow(color*NdotV, vec3(1.0/gamma)), vertex_vs.z);
}