#version 330 core

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

in vec4 worldspace_position;

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

//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
vec3 detailedTexNormal(vec3 viewNormal, vec3 tNormal, vec3 position)
{
    vec3 dPosX  = dFdx(position.xyz);
    vec3 dPosY  = dFdy(position.xyz);
    vec2 dTexX = dFdx(texCoord);
    vec2 dTexY = dFdy(texCoord);

    vec3 normal = normalize(viewNormal);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * tNormal);
}

void main (void)
{   
	gPosition = myprojection_matrix * myview_matrix * worldspace_position;
	gPosition.a = texture(texAO, texCoord.st).r;
	gAlbedo.rgb = pow(texture(texAlbedo, texCoord.st).rgb, vec3(2.2));
	gAlbedo.a = texture(texRough, texCoord.st).r;

	vec3 tNormal = normalize(texture(texNormal, texCoord.st).rgb * 2.0f - 1.0f);
	gNormal.rgb = detailedTexNormal(mynormal_matrix * mynormal, tNormal, worldspace_position.xyz);
	gNormal.a = texture(texMetal, texCoord.st).r;
}