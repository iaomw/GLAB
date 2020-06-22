#version 460 core
layout(location = 0) in vec4 vertex_ms;

uniform mat4 model_matrix;

out vec4 myvertex;
out vec3 mynormal;
out vec3 texCoord;

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

void main()
{
    myvertex = vertex_ms;
    texCoord = vertex_ms.xyz;
    gl_Position = projection_matrix * view_matrix * model_matrix * vertex_ms;
}  