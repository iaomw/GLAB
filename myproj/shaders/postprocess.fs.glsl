#version 460 core

layout (location = 0) out vec4 gColor;

uniform float gamma;
uniform float exposure;
uniform float background;

uniform sampler2D colortex;
//uniform sampler2D gPosition;

uniform sampler2D gEnv;
uniform sampler2D gExtra;
uniform sampler2D gBloom;

uniform bool tonemapping_object;
uniform bool tonemapping_background;

in vec2 texCoords;

vec3 CEToneMapping(vec3 color, float adapted_lum) 
{
    return 1 - exp(-adapted_lum * color);
}

float CEToneMapping(float color, float adapted_lum) 
{
    return 1 - exp(-adapted_lum * color);
}

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main()
{   
    vec4 env = texture(gEnv, texCoords).rgba;
    //env.rgb =  textureLod(gEnv, texCoords, 512).rgb;
    vec4 obj = texture(colortex, texCoords).rgba;
    vec4 bloom = texture(gBloom, texCoords).rgba;
    
    vec3 center = textureLod(gEnv, vec2(0.5, 0.5), 512).rgb;
    float luminance = dot(center, vec3(0.2126, 0.7152, 0.0722));
    float mapped = CEToneMapping(luminance, 1.0);
    mapped = 1.0 - clamp(mapped, 0.0, 0.96);
    // HDR tonemapping
    env.rgb = tonemapping_background? ACESToneMapping(env.rgb, exposure * mapped) : env.rgb;
    obj.rgb = tonemapping_object? ACESToneMapping(obj.rgb, exposure * mapped) : obj.rgb;
    env.rgb *= background;

    vec3 result;
    if (obj.a == 1.0) {
        result = env.rgb + bloom.rgb;
    } else {
        result = (obj.a > env.a) ? obj.rgb : env.rgb; 
        result += (bloom.a > obj.a) ? bloom.rgb : vec3(0);
    }

    result += texture(gExtra, texCoords).rgb;
    // Gamma correct
    result = pow(result, vec3(1.0/gamma));

    gColor.rgb = clamp(result, 0, 1);
    gColor.a = 1.0;
}