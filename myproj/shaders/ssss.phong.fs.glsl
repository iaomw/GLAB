#version 330 core

uniform sampler2D texAlbedo;
uniform sampler2D texNormal;
uniform samplerCube shadowCube;

//layout(location = 0)

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

smooth in vec2 coordinate;
smooth in vec4 vertex_viewspace;
smooth in vec3 normal_viewspace;

uniform float gamma;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

struct Light {
    int type;
    vec3 color;
    vec3 position;
    vec3 intensity;	
    vec3 direction;
	//samplerCube shadow;
};

uniform int num_lights;
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform float kD;
uniform float kS;

uniform float translucency;
uniform float texture_coefficient;

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

vec3 detailedTexNormal(vec3 normal_vspace, vec3 pos_vspace, vec3 tNormal)
{ 
    vec3 dPosX  = dFdx(pos_vspace.xyz);
    vec3 dPosY  = dFdy(pos_vspace.xyz);
    vec2 dTexX = dFdx(coordinate);
    vec2 dTexY = dFdy(coordinate);

    vec3 normal = normalize(normal_vspace.xyz);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * tNormal);
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
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;

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

vec3 T(float s) {
  return vec3(0.233, 0.455, 0.649) * exp(-s * s / 0.0064) +
         vec3(0.1,   0.336, 0.344) * exp(-s * s / 0.0484) +
         vec3(0.118, 0.198, 0.0)   * exp(-s * s / 0.187)  +
         vec3(0.113, 0.007, 0.007) * exp(-s * s / 0.567)  +
         vec3(0.358, 0.004, 0.0)   * exp(-s * s / 1.99)   +
         vec3(0.078, 0.0,   0.0)   * exp(-s * s / 7.41);
}

void main (void)
{   
	vec3 frag_pos = vertex_viewspace.xyz;

	vec3 tColor = texture(texAlbedo, coordinate.st).rgb; 
	tColor = pow(tColor, vec3(gamma));
	gColor.rgb = texture_coefficient * tColor;

	float mark =  texture(texAlbedo, coordinate.st).a;

	vec3 tNormal = texture(texNormal, coordinate).rgb; 
	tNormal = normalize(tNormal * 2.0 - 1.0);   
	vec3 bNormal = detailedTexNormal(normal_viewspace, frag_pos, tNormal);

	frag_pos = frag_pos - 0.005 * bNormal;

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	for (int i=0; i<1; i++) {

		vec3 light_pos = (view_matrix * vec4(lights[i].position, 1.0)).xyz;

		vec3 lightdir = frag_pos - light_pos; 

		ShadowResult shadow = shadowCalculation(frag_pos, light_pos, shadowCube);               
    	//vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;  

		float distance = shadow.delta;

		//if (distance > 0.005) {
			float S = distance; //scale;
			float irradiance = max(0.3 + dot(bNormal, lightdir), 0.0);
			vec3 transmittance = T(S) * lights[i].color * translucency * irradiance * tColor.rgb;

			diffuse += transmittance;
			//gColor.rgb = transmittance;
		//}
		
	    PhongResult phong = computeColor( light_pos, lights[i].color, 
									vec3(kD), vec3(kS), bNormal.xyz, 
		  							vertex_viewspace.xyz, vec3(0.0) );

		diffuse += (1.0 - shadow.shadow) * mark * phong.diffuse;
		specular += (1.0 - shadow.shadow) * mark * phong.specular;
	}

	gColor.rgb += diffuse;
	gExtra.rgb *= specular;

	gColor.a = vertex_viewspace.z;
	gExtra.a = mark;
} 

