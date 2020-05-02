#version 460 core

const float pos_infinity = uintBitsToFloat(0x7F800000);
const float neg_infinity = uintBitsToFloat(0xFF800000);

uniform float fovY;
uniform float farZ;
uniform float nearZ;

uniform float range;
uniform vec2 direction;
uniform float lightSize;

in vec2 texCoords;

uniform sampler2D colortex;
//uniform sampler2D depthtex;

layout (location = 0) out vec4 gColor;
//layout (location = 1) out vec4 gExtra;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * nearZ * farZ) / (farZ + nearZ - z * (farZ - nearZ));
}

void main()
{          
    vec4 result;
    float depth = textureLod(colortex, texCoords, 0).a;
   
    if (depth < -farZ) { // background
        //depth = -farZ; //neg_infinity;
        result.rgba = vec4(0, 0, 0, depth);
    } else {
        result.a = depth;
        result.rgb = 2 * textureLod(colortex, texCoords, 0).rgb * weight[0];
    }

    ivec2 imageSize = textureSize(colortex, 0); 
    //float distanceToCamera = imageSize.x * 0.89629549346f;
    float distanceToCamera = imageSize.x * 0.5f / tan(0.5f * fovY); // pixel length
    float scale = distanceToCamera / abs(depth); // pixel length / viewspace length

    // pixel size will be super small for background, since it's divided by depth.
    float pixelSize = max(2.0f,  range * scale * lightSize);
    vec2 tex_offset = pixelSize / vec2(imageSize.x, imageSize.y);
    tex_offset *= direction;

    vec3 bloom = vec3(0.0);
    float max_z = result.a;
    float current_z; bool test_z;

    for(int i = 1; i < 5; ++i)
    {   
        vec2 cood_offset = tex_offset * i;

        current_z = textureLod(colortex, texCoords + cood_offset, 0).a;
        test_z = (current_z > -farZ);

        if (test_z) {
            bloom += textureLod(colortex, texCoords + cood_offset, 0).rgb * weight[i];
            max_z = max(max_z, current_z);
        }
       
        current_z = textureLod(colortex, texCoords - cood_offset, 0).a;
        test_z = (current_z > -farZ);

        if (test_z) {
            bloom += textureLod(colortex, texCoords - cood_offset, 0).rgb * weight[i];
            max_z = max(max_z, current_z);
        }
    }

    result.rgb += bloom;
    gColor.rgb = result.rgb;

    gColor.a = (max_z > -farZ)? max_z : -2*farZ; 
}