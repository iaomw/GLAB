#version 460 core
#extension GL_ARB_bindless_texture: require

layout (location = 0) out vec4 outAlbedo;
layout (location = 1) out vec4 outNormal;

flat in uint drawID;

in vec4 vertex_vs;
in vec3 normal_vs;
in vec2 texcoord;

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

struct PBR_Unit {
	uvec2 ao;
	uvec2 base;
	uvec2 metalness;
	uvec2 normal;
	uvec2 roughness;
};

layout(std430) buffer PBR_Pack 
{
    uint pbr_pack_size;
    PBR_Unit pbr_pack[];
};

// Calculation in view space
vec3 detailedTexNormal(vec3 normal_vspace, vec3 tNormal, vec3 position)
{ 
    vec3 dPosX  = dFdx(position.xyz);
    vec3 dPosY  = dFdy(position.xyz);
    vec2 dTexX = dFdx(texcoord);
    vec2 dTexY = dFdy(texcoord);

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

vec2 encode (vec3 n)
{
    float f = sqrt(8*n.z+8);
    return n.xy / f + 0.5;
}

vec3 decode (vec2 enc)
{
    vec2 fenc = enc*4-2;
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return n;
}

void main (void)
{   
    //sampler2D texAO = sampler2D(pbr_pack[drawID].ao);
    sampler2D texBase = sampler2D(pbr_pack[drawID].base);
    sampler2D texMetal = sampler2D(pbr_pack[drawID].metalness);
    sampler2D texNormal = sampler2D(pbr_pack[drawID].normal);
    sampler2D texRough = sampler2D(pbr_pack[drawID].roughness);

	outAlbedo.rgb = pow(texture(texBase, texcoord.st).rgb, vec3(gamma));
	outAlbedo.a = texture(texRough, texcoord.st).r;

	vec3 tNormal = normalize(texture(texNormal, texcoord.st).xyz * 2.0f - 1.0f);
	outNormal.rgb = detailedTexNormal(normal_vs.xyz, tNormal, vertex_vs.xyz);
	outNormal.a = texture(texMetal, texcoord.st).r;

	outNormal.rg = encode(outNormal.rgb);
	outNormal.b = vertex_vs.z;
}