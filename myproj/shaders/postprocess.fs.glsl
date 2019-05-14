#version 330 core

layout (location = 0) out vec4 gColor;

uniform sampler2D tex;
uniform sampler2D gEnv;

in vec2 texCoords;

void main()
{   
    vec4 result = texture(tex, texCoords).rgba;
    if (result.a < 1.0) 
        result = texture(gEnv, texCoords);
     // HDR tonemapping
    result = result / (result + vec4(1.0));
    // Gamma correct
    result = pow(result, vec4(1.0/2.2));

    gColor = result;
    gColor.a = 1.0;
}