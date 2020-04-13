#version 440 core

in vec2 texCoords;

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;

uniform sampler2D gAlbedo; //linear space
uniform sampler2D gPosition; //linear space

uniform float sssWidth;
uniform vec2 direction;
uniform float depthThreshold;

uniform int kernelSize;
uniform vec4 kernel[32];

uniform float fovY;
uniform float farZ;
uniform float nearZ;

uniform vec3 cam_position;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

void main()
{   
    vec3 cam_vspace = (myview_matrix * vec4(cam_position, 1.0)).xyz; 

    vec4 colorM = texture(gAlbedo, texCoords); 
    colorM.rgb *= vec3(colorM.a);

    float luminance = 0.212*colorM.r + 0.7152*colorM.g + 0.0722*colorM.b;

    if (luminance > 0.05) {

    float SSSS_STREGTH_SOURCE = colorM.a;
    if (SSSS_STREGTH_SOURCE==0.0)  discard;

    vec3 tPosition = texture(gPosition, texCoords).rgb;
    float depthM = texture(gPosition, texCoords).w;
    if (tPosition.x == 0.0) discard;

    float scale = (texCoords.s - 0.5) / tPosition.x; // ST space : View space;

    // Calculate the final step to fetch the surrounding pixels:
    vec2 finalStep = direction * sssWidth * scale;
    finalStep *= SSSS_STREGTH_SOURCE; // Modulate it using the alpha channel.
    finalStep *= 1.0 / 3.0; // Divide by 3 as the kernels range from -3 to 3.

    // Accumulate the center sample:
    vec4 colorBlurred = colorM;
    colorBlurred.rgb *= kernel[0].rgb;

    // Accumulate the other samples:
    for (int i = 1; i < kernelSize; i++) {
        // Fetch color and depth for current sample:
        vec2 offset = texCoords + kernel[i].a * finalStep;
        vec4 color = texture(gAlbedo, offset);
        color.rgb *= vec3(color.a);

        //#if SSSS_FOLLOW_SURFACE == 1
        // If the difference in depth is huge, we lerp color back to "colorM":
        float depth = texture(gPosition, offset).w;
        //float s = SSSSSaturate(300.0f * distanceToProjectionWindow * sssWidth * abs(depthM - depth));
        //float s = 300.0f * distanceToProjectionWindow * sssWidth * abs(depthM - depth);

        float s = depthThreshold * sssWidth * scale * abs(depthM - depth);
        s = clamp(s, 0.0, 1.0);

        //color.rgb = SSSSLerp(color.rgb, colorM.rgb, s);
        color.rgb = mix(color.rgb, colorM.rgb, s);
        //#endifs

        // Accumulate:
        colorBlurred.rgb += kernel[i].rgb * color.rgb;
    }
    gColor.rgb = colorBlurred.rgb;
    gColor.a = 1.0; //texture(gPosition, texCoords).w;

    } else {

        gColor.rgb = colorM.rgb;
        gColor.a = 1.0; 
    }

    gExtra.rg = texCoords;
}