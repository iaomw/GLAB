#version 330 core

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

in vec4 vertex_viewspace;
in vec3 normal_viewspace;

in vec4 myvertex;
in vec3 mynormal;
in vec2 texCoord;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gAlbedo;
layout (location = 2) out vec4 gNormal;

uniform sampler2D texAO;
uniform sampler2D texAlbedo;
uniform sampler2D texMetal;
uniform sampler2D texNormal;
uniform sampler2D texRough;

uniform float fovY;
uniform float farZ;
uniform float nearZ;
uniform float gamma;

// Calculation in view space
vec3 detailedTexNormal(vec3 normal_vspace, vec3 tNormal, vec3 position)
{ 
    vec3 dPosX  = dFdx(position.xyz);
    vec3 dPosY  = dFdy(position.xyz);
    vec2 dTexX = dFdx(texCoord);
    vec2 dTexY = dFdy(texCoord);

    vec3 normal = normalize(normal_vspace);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * tNormal);
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * nearZ * farZ) / (farZ + nearZ - z * (farZ - nearZ));
}

void main (void)
{   
	gPosition.rgb = vertex_viewspace.xyz;
	gPosition.a = texture(texAO, texCoord.st).r;
    //gPosition.z = LinearizeDepth(gl_FragCoord.z);

	gAlbedo.rgb = pow(texture(texAlbedo, texCoord.st).rgb, vec3(gamma));
	gAlbedo.a = texture(texRough, texCoord.st).r;

	vec3 tNormal = normalize(texture(texNormal, texCoord.st).rgb * 2.0f - 1.0f);
	gNormal.rgb = detailedTexNormal(normal_viewspace.xyz, tNormal, vertex_viewspace.xyz);
	gNormal.a = texture(texMetal, texCoord.st).r;
}