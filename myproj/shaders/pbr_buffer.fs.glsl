#version 460 core
#extension GL_ARB_bindless_texture: require

const float PI  = 3.14159265358979323846264338327950288419716939937510f;

layout(location = 0) out vec4 gColor;

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

layout(std430) buffer PBR_Pass {
   
    uvec2 texIrradiance;
	uvec2 texPrefilter;
	uvec2 texBRDF;

    uvec2 complex;
    uvec2 albedo;
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

float saturate(float f)
{
    return clamp(f, 0.0f, 1.0f);
}

vec3 FresnelSchlick(float NdotV, vec3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - NdotV, 5.0f);
}

vec3 FresnelRoughness(float NdotV, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - NdotV, 5.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float rough2 = roughness * roughness;
    float rough4 = rough2 * rough2;

    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;

    float denom  = (NdotH2 * (rough4 - 1.0) + 1.0);
    denom        = PI * denom * denom;

    return rough4 / denom;
}

float SmithGeometryGGX(float NdotL, float NdotV, float roughness)
{
    float NdotL2 = NdotL * NdotL;
    float NdotV2 = NdotV * NdotV;
    float kRough2 = roughness * roughness + 0.0001f;

    float ggxL = (2.0f * NdotL) / (NdotL + sqrt(NdotL2 + kRough2 * (1.0f - NdotL2)));
    float ggxV = (2.0f * NdotV) / (NdotV + sqrt(NdotV2 + kRough2 * (1.0f - NdotV2)));

    return ggxL * ggxV;
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

void main()
{             
    vec4 texAlbedo = texture(sampler2D(albedo), texcoord).rgba;
    vec4 texNormal = texture(sampler2D(complex), texcoord).rgba;

    float mark = texAlbedo.a;
    float texDepth = texNormal.z;

    if (1.0 == mark && 0 == texDepth) { 
        gColor.a = 1.0;
        return; //discard; 
    }

    float roughness = texAlbedo.a;
    float metalness = texNormal.a;

    vec4 tPosition = vec4(1.0);

    vec2 offset = 2.0f * texcoord - vec2(1.0f);
    float unit = -texDepth * tan(fovY/2);

    offset.x = offset.x * unit * XdY;
    offset.y = offset.y * unit;
    
    tPosition.xy = offset.xy; 
    tPosition.z = texDepth;

    vec3 theNormal = decode(texNormal.rg);

    float ao = 1.0; //texture(ao, texcoord).a;

    vec3 V = normalize(vec3(0.0) - tPosition.xyz);
    vec3 N = normalize(theNormal);
    vec3 R = reflect(-V, N);

    float NdotV = max(dot(N, V), 0.0001f);
    vec3 materialF0 = vec3(0.04f); // for non-metal

    // Fresnel (Schlick) computation (F term)
    vec3 F0 = mix(materialF0, texAlbedo.rgb, metalness);
    vec3 F = FresnelRoughness(NdotV, F0, roughness);

    // Energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0)-kS;
    kD *= 1.0f - metalness;

    float atten = 0.01;
    vec3 color = vec3(0.0f); 
    vec3 diffuse = vec3(0.0f);  
    vec3 specular = vec3(0.0f);
    
    float c2f = distance(vec3(0.0), tPosition.xyz); // view space 
    for (int i = 0; i < lightCount; i++) // treate them as point lights
    {
        vec4 light_pos = view_matrix * vec4(lightList[i].position.xyz, 1.0); // world space to view space
        vec3 direction = (light_pos-tPosition).xyz;
        vec3 L = normalize(direction);
        vec3 H = normalize(L + V);

        vec3 lightColor = lightList[i].color.rgb; 
        vec3 intensity = lightList[i].intensity.rgb;
        float l2f = distance(light_pos.xyz, tPosition.xyz);

        atten = 1.0 / l2f; //based on distance
        // Light source dependent BRDF term(s)
        float NdotL = saturate(dot(N, L));
        diffuse = texAlbedo.rgb / PI; // Lambertian
        //diffuse *= kDisneyTerm(NdotL, NdotV, roughness);
        
        float D = DistributionGGX(N, H, roughness);
        float G = SmithGeometryGGX(NdotL, NdotV, roughness);

        vec3 FS = FresnelSchlick(max(0.0, dot(H, V)), F0);

        specular = (D * FS * G) / (4.0f * NdotL * NdotV + 0.0001f);
        color += (diffuse * kD + specular) * lightColor * atten * NdotL; 
     }

    vec4 world_N = weiv_matrix * vec4(N, 0);
    vec4 world_R = weiv_matrix * vec4(R, 0);

    vec3 irradiance = texture(samplerCube(texIrradiance), world_N.xyz).rgb;
    vec3 value_diffuse = irradiance * texAlbedo.rgb;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 5.0;
    float mip_level = roughness * MAX_REFLECTION_LOD;
    vec2 value_BRDF = texture(sampler2D(texBRDF), vec2(NdotV, roughness)).rg;
    vec3 value_prefilter = textureLod(samplerCube(texPrefilter), world_R.xyz, mip_level).rgb; 
    vec3 value_specular = value_prefilter * (F * value_BRDF.x + value_BRDF.y);
       
    vec3 ambient = (kD * value_diffuse + value_specular) * ao;

    //gColor.rgb = value_prefilter; 
    gColor.rgb = color + ambient;
    gColor.a = tPosition.z;
}