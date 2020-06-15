#version 330 core

const float PI  = 3.14159265358979323846264338327950288419716939937510f;

layout (location = 0) out vec4 gColor;

uniform mat4 weiv_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

uniform float fovY;
uniform float farZ;
uniform float nearZ;

in vec4 myvertex;
in vec3 mynormal;
in vec2 texCoords;

struct Light {
    int type;
    vec3 color;
    vec3 position;
    vec3 intensity;	
    vec3 direction;
};

uniform int num_lights;
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;

uniform samplerCube gIrradiance;
uniform samplerCube gPrefilter;
uniform sampler2D BRDF_LUT;

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

bool bad(vec3 value) {

    bvec3 result = isinf(value); 

    if (result.x || result.y || result.z) {
        return true;
    }
    
    result = isnan(value);
    if (result.x || result.y || result.z) {
        return true;
    }

    return false;
}

void main()
{             
    // retrieve data from gbuffer
    vec4 tPosition = texture(gPosition, texCoords);
    if (0 == tPosition.z && 1.0 == tPosition.a) { 
        gColor.a = 1.0;
        //discard; 
        return; 
    }

    vec3 texAlbedo = texture(gAlbedo, texCoords).rgb;
    vec3 texNormal = texture(gNormal, texCoords).rgb;

    float roughness = texture(gAlbedo, texCoords).a;
    float metalness = texture(gNormal, texCoords).a;
    float ao = texture(gPosition, texCoords).a;

    vec3 V = normalize(vec3(0.0) - tPosition.xyz);
    vec3 N = normalize(texNormal);
    vec3 R = reflect(-V, N);

    float NdotV = max(dot(N, V), 0.0001f);
    vec3 materialF0 = vec3(0.04f); // for non-metal

    // Fresnel (Schlick) computation (F term)
    vec3 F0 = mix(materialF0, texAlbedo, metalness);
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
    for (int i = 0; i < num_lights; i++) // treate them as point lights
    {
        vec4 light_pos = view_matrix * vec4(lights[i].position, 1.0); // world space to view space
        vec3 direction = (light_pos-tPosition).xyz;
        vec3 L = normalize(direction);
        vec3 H = normalize(L + V);

        vec3 lightColor = lights[i].color; 
        vec3 intensity = lights[i].intensity;
        float l2f = distance(light_pos.rgb, tPosition.rgb);

        atten = 1.0 / l2f; //based on distance
        // Light source dependent BRDF term(s)
        float NdotL = saturate(dot(N, L));
        diffuse = texAlbedo / PI; // Lambertian
        //diffuse *= kDisneyTerm(NdotL, NdotV, roughness);
        
        float D = DistributionGGX(N, H, roughness);
        float G = SmithGeometryGGX(NdotL, NdotV, roughness);

        vec3 FS = FresnelSchlick(max(0.0, dot(H, V)), F0);

        specular = (D * FS * G) / (4.0f * NdotL * NdotV + 0.0001f);
        color += (diffuse * kD + specular) * lightColor * atten * NdotL; 
     }

    vec4 world_N = weiv_matrix * vec4(N, 0);
    vec4 world_R = weiv_matrix * vec4(R, 0);

    vec3 irradiance = texture(gIrradiance, world_N.xyz).rgb;
    vec3 value_diffuse = irradiance * texAlbedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 5.0;
    float mip_level = roughness * MAX_REFLECTION_LOD;
    vec2 value_BRDF = texture(BRDF_LUT, vec2(NdotV, roughness)).rg;
    vec3 value_prefilter = textureLod(gPrefilter, world_R.xyz, mip_level).rgb; 
    vec3 value_specular = value_prefilter * (F * value_BRDF.x + value_BRDF.y);
       
    vec3 ambient = (kD * value_diffuse + value_specular) * ao;

    //gColor.rgb = value_prefilter; 
    gColor.rgb = color + ambient;
    gColor.a = tPosition.z;
}