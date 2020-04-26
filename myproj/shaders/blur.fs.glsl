#version 440 core

const float pos_infinity = uintBitsToFloat(0x7F800000);
const float neg_infinity = uintBitsToFloat(0xFF800000);

in vec2 TexCoords;

uniform sampler2D gAlbedo;
//uniform sampler2D gPosition;

layout (location = 0) out vec4 gColor;
//layout (location = 1) out vec4 gExtra;

uniform float fovY;
uniform float farZ;
uniform float nearZ;

uniform float range;
uniform vec2 direction;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{          

    vec4 result;
    float depth = texture(gAlbedo, TexCoords).a;
    if (1.0 == depth) { // background
        result.rgba = vec4(0, 0, 0, neg_infinity);
        depth = neg_infinity;
    } else {
        result.rgb = 2 * texture(gAlbedo, TexCoords).rgb * weight[0];
        result.a = texture(gAlbedo, TexCoords).a;
    }

    float distanceToCamera = 0.5 * 1.0 / tan(0.5 * fovY);
    float scale = distanceToCamera / abs(depth);

    vec2 tex_offset = range / textureSize(gAlbedo, 0); // gets size of texel
    tex_offset *= direction;

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

    gColor.a = (max_z > neg_infinity)? max_z : 1.0; 
}