#version 460 core

const float pos_infinity = uintBitsToFloat(0x7F800000);
const float neg_infinity = uintBitsToFloat(0xFF800000);

layout (location = 0) out vec4 gColor;

uniform mat4 model_matrix;
uniform mat3 normal_matrix;

uniform samplerCube cubetex;

in vec4 myvertex;
in vec3 mynormal;
in vec3 texCoord;

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
     //gExtra = view_matrix * model_matrix * myvertex; 
     gColor = texture(cubetex, texCoord);
     gColor.a = -2*farZ; // background
    
     //float brightness = dot(gColor.rgb, vec3(0.2126, 0.7152, 0.0722));
     //gColor.w = (brightness>1.0)? 1.0:0.0;
}