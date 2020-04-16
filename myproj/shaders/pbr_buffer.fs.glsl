#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

const float PI  = 3.14159265358979323846264338327950288419716939937510f;

uniform vec3 cam_position;

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

uniform mat4 inverse_view_matrix;

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

float Fd90(float NoL, float roughness)
{
    return (2.0f * NoL * roughness) + 0.4f;
}

float KDisneyTerm(float NoL, float NoV, float roughness)
{
    return (1.0f + Fd90(NoL, roughness) * pow(1.0f - NoL, 5.0f)) * (1.0f + Fd90(NoV, roughness) * pow(1.0f - NoV, 5.0f));
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

void main()
{             
    // retrieve data from gbuffer
    vec4 tPosition = texture(gPosition, texCoords);
    if (tPosition == vec4(0.0)) { discard; }

    vec3 tAlbedo = texture(gAlbedo, texCoords).rgb;
    vec3 tNormal = texture(gNormal, texCoords).rgb;

    float roughness = texture(gAlbedo, texCoords).a;
    float metalness = texture(gNormal, texCoords).a;
    float ao = texture(gPosition, texCoords).a;

    vec3 cam_vspace = (myview_matrix * vec4(cam_position, 1.0)).xyz; 

    vec3 V = normalize(cam_vspace - tPosition.rgb);
    vec3 N = normalize(tNormal);
    vec3 R = reflect(-V, N);

    float NdotV = max(dot(N, V), 0.0001f);
    vec3 materialF0 = vec3(0.04f); // for non-metal

    // Fresnel (Schlick) computation (F term)
    vec3 F0 = mix(materialF0, tAlbedo, metalness);
    vec3 F = FresnelRoughness(NdotV, F0, roughness);

    // Energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0)-kS;
    kD *= 1.0f - metalness;

    float Atten = 0.01;
    vec3 color = vec3(0.0f); 
    vec3 diffuse = vec3(0.0f);  
    vec3 specular = vec3(0.0f);
    
    float c2p = distance((myview_matrix * vec4(cam_vspace, 1.0)).xyz, tPosition.xyz); // view space 
    // Calculation in view space
    for (int i = 0; i < num_lights; i++) // treate them as point lights
    {
        vec4 light_pos = myview_matrix * vec4(lights[i].position, 1.0); // world space to view space
        vec3 direction = (light_pos-tPosition).rgb;
        vec3 L = normalize(direction);
        vec3 H = normalize(L + V);

        vec3 lightColor = lights[i].color; 
        vec3 intensity = lights[i].intensity;
        float l2p = distance(light_pos.rgb, tPosition.rgb);
        vec3 remain = intensity * max((1-(l2p+c2p) * Atten), 0.0); 

        // Light source dependent BRDF term(s)
        float NdotL = saturate(dot(N, L));
        diffuse = tAlbedo / PI; // Lambertian
        float kDisney = KDisneyTerm(NdotL, NdotV, roughness);
        diffuse = diffuse * kDisney;
        float D = DistributionGGX(N, H, roughness);
        float G = SmithGeometryGGX(NdotL, NdotV, roughness);

        specular = (D * F * G) / (4.0f * NdotL * NdotV + 0.0001f);
        color += (diffuse * kD + specular * kS) * lightColor * remain * NdotL; 
     }

    vec4 world_N = inverse_view_matrix * vec4(N, 0);
    vec4 world_R = inverse_view_matrix * vec4(R, 0);

    vec3 irradianceX = texture(gIrradiance, world_N.xyz).rgb;
    vec3 diffuseX = irradianceX * tAlbedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(gPrefilter, world_R.xyz, roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 xBRDF  = texture(BRDF_LUT, vec2(NdotV, roughness)).rg;
    vec3 specularX = prefilteredColor * (F * xBRDF.x + xBRDF.y);

    vec3 ambient = (kD * diffuseX + kS * specularX) * ao;

    gColor.rgb = color + ambient;
    //gColor.rgb = diffuseX;
    //gColor.rgb = specularX;
    gColor.a = 1.0;

    //gExtra = tPosition;
}