#version 330 core

layout (location = 0) out vec4 gColor;

uniform sampler2D tex;
uniform sampler2D gEnv;

uniform sampler2D gExtra;
uniform sampler2D gPosition;


in vec2 texCoords;

void main()
{   
    vec4 obj = texture(tex, texCoords).rgba;
    vec4 env = texture(gEnv, texCoords).rgba;
    vec3 pos = texture(gPosition, texCoords).rgb;

    vec3 result;
    if (obj.a < 1.0) {
        result = env.rgb;
    } else { // compare view space z position
        float bz = texture(gExtra, texCoords).z; 
        result = pos.z > bz ? obj.rgb : env.rgb;
    }
     // HDR tonemapping
    result = result / (result + vec3(1.0));
    // Gamma correct
    result = pow(result, vec3(1.0/2.2));

    gColor.rgb = result;
    gColor.a = 1.0;
}