#version 460 core 
#extension GL_ARB_bindless_texture: require   
                         
uniform sampler2D colortex;
uniform sampler2D normaltex;

smooth in vec2 texcoord;
smooth in vec3 normal_vs;
smooth in vec4 vertex_vs;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

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

uniform float kD;
uniform float kS;

uniform float ambient;
uniform float translucency;

uniform float shadow_bias;
uniform float distance_scale;

struct PhongResult {
	vec3 specular;
	vec3 diffuse;
};

PhongResult phongColor(vec3 lightpos, vec3 lightcolor, vec3 normal, vec3 mypos)
{
	vec3 eyepos = vec3(0.0f);

	vec3 mypos_to_eyepos = normalize(eyepos - mypos);
    vec3 mypos_to_lightpos = normalize(lightpos - mypos);

	vec3 reflection = normalize( -mypos_to_lightpos + (2 * dot(mypos_to_lightpos, normal)) * normal );

	PhongResult result;
	 
	result.diffuse = lightcolor * kD * max(dot(normal, mypos_to_lightpos), 0);
	result.specular = lightcolor * kS * pow(max(dot(reflection, mypos_to_eyepos), 0), 20);

	return result;
}

vec3 richNormal(vec3 normal_vspace, vec3 pos_vspace, vec3 normaltex)
{ 
    vec3 dPosX  = dFdx(pos_vspace.xyz);
    vec3 dPosY  = dFdy(pos_vspace.xyz);
    vec2 dTexX = dFdx(texcoord);
    vec2 dTexY = dFdy(texcoord);

    vec3 normal = normalize(normal_vspace.xyz);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * normaltex);
}

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

struct ShadowResult {
	float shadow;
	float delta;
};

ShadowResult shadowCalculation(vec3 fragPos, vec3 lightPos, samplerCube depthMap)
{
    // get vector between fragment position and light position
    vec3 fragToLight = (fragPos - lightPos);

	fragToLight = ( weiv_matrix * vec4(fragToLight, 0.0) ).xyz;
	//fragToLight = ( inverse(view_matrix) * vec4(fragToLight, 0.0) ).xyz;

    float currentDepth = length(fragToLight);

    float bias = shadow_bias; //0.5;
    int samples = 20;
	float shadow = 0.0;

	vec3 viewPos = vec3(0.0);
	float far_plane = 1000.0f;

    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples); 

	ShadowResult result;
	result.shadow = shadow;

	float depth = texture(depthMap, fragToLight).r;
	result.delta = currentDepth - depth * far_plane;
        
    return result;
}

vec3 T(float s) { // It doesn't matter, positive or negative value.
  return vec3(0.233, 0.455, 0.649) * exp(-s * s / 0.0064) +
         vec3(0.1,   0.336, 0.344) * exp(-s * s / 0.0484) +
         vec3(0.118, 0.198, 0.0)   * exp(-s * s / 0.187)  +
         vec3(0.113, 0.007, 0.007) * exp(-s * s / 0.567)  +
         vec3(0.358, 0.004, 0.0)   * exp(-s * s / 1.99)   +
         vec3(0.078, 0.0,   0.0)   * exp(-s * s / 7.41);
}

void main (void)
{   
	vec3 texColor = texture(colortex, texcoord).rgb; 
	texColor = pow(texColor, vec3(gamma));

	float mark =  texture(colortex, texcoord).a;
	vec3 frag_pos = vertex_vs.xyz;

	vec3 texNormal = texture(normaltex, texcoord).rgb; 
	texNormal = normalize(texNormal * 2.0 - 1.0);   
	texNormal = richNormal(normal_vs, frag_pos, texNormal);
	//frag_pos = frag_pos - 0.005 * normaltex;

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 transmit = vec3(0.0);
	
	for (int i=0; i<lightCount; ++i) {

		vec3 light_pos = (view_matrix * vec4(lightList[i].position.xyz, 1.0)).xyz;
		vec3 light_dir = normalize(frag_pos - light_pos); 
		//lightdir = -light_pos;

		uvec2 shadow_handle = lightList[i].shadow_handle;

		ShadowResult sr = shadowCalculation(frag_pos, light_pos, samplerCube(shadow_handle));     

		float S = sr.delta * distance_scale;
		float irradiance = max(0.3 + dot(texNormal, light_dir), 0.0);
		vec3 transmittance = T(S) * lightList[i].color.rgb * translucency * irradiance;

		transmit += transmittance;
		
	    PhongResult phong = phongColor(light_pos, lightList[i].color.rgb, texNormal, frag_pos);

		diffuse += (1.0 - sr.shadow) * mark * phong.diffuse;
		specular += (1.0 - sr.shadow) * mark * phong.specular;
	}

	//gColor.rgb = ambient * texColor;
	gColor.rgb += (ambient + diffuse) * texColor;
	gExtra.rgb += (transmit + specular) * texColor;
	//gColor.rgb = texNormal.rgb;

	gColor.a = frag_pos.z;
	gExtra.a = mark;
} 

