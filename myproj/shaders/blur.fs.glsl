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

    vec3 result;
    if (texture(gAlbedo, TexCoords).a < 1.0) {
        result = vec3(0.0);
    } else {
        result = texture(gAlbedo, TexCoords).rgb * weight[0];
    }

    vec3 bloom = vec3(0.0);

     if(horizontal > 0)
     {
         for(int i = 1; i < 5; ++i)
         {
            bloom += texture(gAlbedo, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * texture(gAlbedo, TexCoords + vec2(tex_offset.x * i, 0.0)).a;
            bloom += texture(gAlbedo, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * texture(gAlbedo, TexCoords - vec2(tex_offset.x * i, 0.0)).a;
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
            bloom += texture(gAlbedo, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * texture(gAlbedo, TexCoords + vec2(0.0, tex_offset.y * i)).a;
            bloom += texture(gAlbedo, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * texture(gAlbedo, TexCoords - vec2(0.0, tex_offset.y * i)).a;
         }
     }

    result += bloom;
    gColor.rgb = result;

    gColor.a = (result==vec3(0.0))? 0.0:1.0; 
}