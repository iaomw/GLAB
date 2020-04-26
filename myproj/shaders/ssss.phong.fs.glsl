#version 330 core

uniform sampler2D texAlbedo;
uniform sampler2D texNormal;

//layout(location = 0)

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

in vec2 coordinate;
in vec4 vertex_viewspace;
in vec3 normal_viewspace;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

struct Light {
    int type;
    vec3 color;
    vec3 position;
    vec3 intensity;	
    vec3 direction;
};

uniform float kD;
uniform float kS;

uniform float texture_coefficient;

uniform float gamma;

uniform int num_lights;
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

// uniform struct Material
// {
// 	vec4 kd;
// 	vec4 ks;
// 	vec4 ka;
// 	float specular;
// } material;

struct PhongResult {
	vec3 specular;
	vec3 diffuse;
};

PhongResult computeColor(vec3 _lightpos, vec3 lightcolor,
                  			vec3 kd, vec3 ks,
				 		 vec3 _normal, vec3 _mypos, vec3 _eyepos)
{
	vec3 mypos = _mypos.xyz;
	vec3 eyepos = _eyepos.xyz;
	vec3 lightpos = _lightpos.xyz;
    vec3 normal = normalize(_normal);

    vec3 mypos_to_lightpos = normalize( lightpos - mypos );
	vec3 lightpos_to_mypos = -mypos_to_lightpos;
	vec3 mypos_to_eyepos = normalize(eyepos - mypos);

	vec3 reflection = normalize( lightpos_to_mypos + (2 * dot(mypos_to_lightpos, normal))*normal );

	PhongResult result;
	 
	result.diffuse = lightcolor * kd * max(dot(normal, mypos_to_lightpos), 0);
	result.specular = lightcolor * ks * pow(max(dot(reflection, mypos_to_eyepos), 0), 20);

	return result;
}

vec3 detailedTexNormal(vec3 normal_vspace, vec3 tNormal, vec3 position)
{ 
    vec3 dPosX  = dFdx(position.xyz);
    vec3 dPosY  = dFdy(position.xyz);
    vec2 dTexX = dFdx(coordinate);
    vec2 dTexY = dFdy(coordinate);

    vec3 normal = normalize(normal_vspace);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * tNormal);
}

// float3 SSSSTransmittance(
//         float translucency,
//         float ssssWidth,
//         vec3 worldPosition,
//         vec3 worldNormal,
//         // Light vector: lightWorldPosition - worldPosition.
//         vec3 light,
//         // Linear 0..1 shadow map.
//         SSSSTexture2D shadowMap,
//         // Regular world to light space matrix.
//         mat4 lightViewProjection,
//         // Far plane distance used in the light projection matrix.
//         float lightFarPlane) {
    
//     float scale = 8.25 * (1.0 - translucency) / ssssWidth;
       
//     vec4 shrinkedPos = float4(worldPosition - 0.005 * worldNormal, 1.0);

//     /**
//      * Now we calculate the thickness from the light point of view:
//      */
//     vec4 shadowPosition = SSSSMul(shrinkedPos, lightViewProjection);
//     float d1 = SSSSSample(shadowMap, shadowPosition.xy / shadowPosition.w).r; // 'd1' has a range of 0..1
//     float d2 = shadowPosition.z; // 'd2' has a range of 0..'lightFarPlane'
//     d1 *= lightFarPlane; // So we scale 'd1' accordingly:
//     float d = scale * abs(d1 - d2);

//     /**
//      * Armed with the thickness, we can now calculate the color by means of the
//      * precalculated transmittance profile.
//      * (It can be precomputed into a texture, for maximum performance):
//      */
//     float dd = -d * d;
//     vec3 profile = vec3(0.233, 0.455, 0.649) * exp(dd / 0.0064) +
//                 	vec3(0.1,   0.336, 0.344) * exp(dd / 0.0484) +
//                     vec3(0.118, 0.198, 0.0)   * exp(dd / 0.187)  +
//                     vec3(0.113, 0.007, 0.007) * exp(dd / 0.567)  +
//                     vec3(0.358, 0.004, 0.0)   * exp(dd / 1.99)   +
//                     vec3(0.078, 0.0,   0.0)   * exp(dd / 7.41);

//     /** 
//      * Using the profile, we finally approximate the transmitted lighting from
//      * the back of the object:
//      */
//     return profile * SSSSSaturate(0.3 + dot(light, -worldNormal));
// }

void main (void)
{   
	vec3 tColor = texture_coefficient * texture(texAlbedo, coordinate.st).rgb; 
	float mark =  texture(texAlbedo, coordinate.st).a;
	gColor.rgb = pow(tColor, vec3(gamma));

	vec3 tNormal = texture(texNormal, coordinate).rgb;
	vec3 bNormal = detailedTexNormal(normal_viewspace.xyz, tNormal, vertex_viewspace.xyz);
	for (int i=0; i<3; i++) {
		vec3 light_pos = (myview_matrix * vec4(lights[i].position, 1.0)).xyz;
	    PhongResult phong = computeColor( light_pos, vec3(0.7),//lights[i].color, 
                            	vec3(kD), vec3(kS), normal_viewspace.xyz, 
		  							vertex_viewspace.xyz, vec3(0,0,0) );
		gColor.rgb += mark*phong.diffuse;
		gExtra.rgb += mark*phong.specular;
	}
	gColor.a = vertex_viewspace.z;
	gExtra.a = mark;
} 

