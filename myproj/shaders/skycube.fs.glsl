#version 330 core

const float pos_infinity = uintBitsToFloat(0x7F800000);
const float neg_infinity = uintBitsToFloat(0xFF800000);

layout (location = 0) out vec4 gColor;
//layout (location = 1) out vec4 gExtra;

in vec4 myvertex;
in vec3 mynormal;
in vec3 texCoord;

uniform float fovY;
uniform float farZ;
uniform float nearZ;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

uniform samplerCube cubetex;

void main()
{    
     //gExtra = view_matrix * model_matrix * myvertex; 
     gColor = texture(cubetex, texCoord);
     gColor.a = -2*farZ; // background
    
     //float brightness = dot(gColor.rgb, vec3(0.2126, 0.7152, 0.0722));
     //gColor.w = (brightness>1.0)? 1.0:0.0;
}