#version 440 core

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38

in vec2 TexCoords;

uniform sampler2D gAlbedo;
uniform sampler2D gPosition;

layout (location = 0) out vec4 gColor;
//layout (location = 1) out vec4 gExtra;

uniform float range;
uniform int horizontal;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
    vec2 tex_offset = range / textureSize(gAlbedo, 0); // gets size of single texel

    vec4 result;
    if (texture(gAlbedo, TexCoords).a >= 0.0) {
        result.rgba = vec4(0, 0, 0, -FLT_MAX);
    } else {
        result.rgb = 2 * texture(gAlbedo, TexCoords).rgb * weight[0];
        result.a = texture(gAlbedo, TexCoords).a;
    }

    vec2 dxdy = (horizontal>0)? vec2(1, 0) : vec2(0, 1);
    tex_offset *= dxdy;

    vec3 bloom = vec3(0.0);
    float max_z = result.a;
    float current_z; bool test_z;

    for(int i = 1; i < 5; ++i)
    {   
        vec2 cood_offset = tex_offset * i;

        current_z = texture(gAlbedo, TexCoords + cood_offset).a;
        test_z = (current_z<0.0);

        if (test_z) {
            bloom += texture(gAlbedo, TexCoords + cood_offset).rgb * weight[i];
            max_z = max(max_z, current_z);
        }
       
        current_z = texture(gAlbedo, TexCoords - cood_offset).a;
        test_z = (current_z<0.0);

        if (test_z) {
            bloom += texture(gAlbedo, TexCoords - cood_offset).rgb * weight[i];
            max_z = max(max_z, current_z);
        }
    }

    result.rgb += bloom;
    gColor.rgb = result.rgb;

    gColor.a = (vec3(0)==result.rgb)? 0.0 : max_z; 
}