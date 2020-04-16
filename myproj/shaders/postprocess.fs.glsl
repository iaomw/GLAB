#version 330 core

layout (location = 0) out vec4 gColor;

uniform float exposure;

uniform sampler2D colortex;
uniform sampler2D gPosition;

uniform sampler2D gEnv;
uniform sampler2D gExtra;
uniform sampler2D gBloom;

in vec2 texCoords;

void main()
{   
    vec4 env = texture(gEnv, texCoords).rgba;
    vec4 obj = texture(colortex, texCoords).rgba;
    vec3 pos = texture(gPosition, texCoords).rgb;
    vec4 bloom = texture(gBloom, texCoords).rgba;

    vec3 result;
    if (obj.a < 1.0) {
        result = env.rgb + bloom.rgb;
    } else { // compare view space z position
        float bz = texture(gExtra, texCoords).z; 
        result = (pos.z > bz) ? obj.rgb : env.rgb;
        result += (bloom.a >= pos.z) ? bloom.rgb : vec3(0);
    }
 
    // Exposure tonemapping
    result = vec3(1.0) - exp(-result * exposure);
     // HDR tonemapping
    result = result / (result + vec3(1.0));
    // Gamma correct
    result = pow(result, vec3(1.0/2.2));

    gColor.rgb = result;
    gColor.a = 1.0;
}