#version 440 core

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
        result.rgb = vec3(0.0);
    } else {
        result.rgb = 2 * texture(gAlbedo, TexCoords).rgb * weight[0];
        result.a = texture(gAlbedo, TexCoords).a;
    }

    vec2 dxdy = (horizontal>0)? vec2(1, 0) : vec2(0, 1);
    tex_offset *= dxdy;

    vec3 bloom = vec3(0.0);
    float max_z = -result.a;

    for(int i = 1; i < 5; ++i)
    {   
        vec2 cood_offset = tex_offset * i;
        bloom += (0.0<=texture(gAlbedo, TexCoords + cood_offset).a)? vec3(0.0):texture(gAlbedo, TexCoords + cood_offset).rgb * weight[i];
        bloom += (0.0<=texture(gAlbedo, TexCoords - cood_offset).a)? vec3(0.0):texture(gAlbedo, TexCoords - cood_offset).rgb * weight[i];
       
        max_z = max(max(max_z, -texture(gAlbedo, TexCoords + cood_offset).a), -texture(gAlbedo, TexCoords - cood_offset).a);
    }

    result.rgb += bloom;
    gColor.rgb = result.rgb;

    gColor.a = (result.rgb==vec3(0.0))? 0.0 : -max_z; 
}