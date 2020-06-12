#version 330 core
 
in vec3 WorldPos;

layout (location = 0) out vec4 gColor;

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
    vec3 color = pow(texture(colortex, uv).rgb, vec3(1.0));

    gColor = vec4(color, 1.0);
}
