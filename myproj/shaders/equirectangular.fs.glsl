#version 330 core
 
in vec3 WorldPos;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

uniform sampler2D colortex;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    vec3 color = texture(colortex, uv).rgb;

    gColor = vec4(color, 1.0);
    gExtra = vec4(color, 1.0);
}
