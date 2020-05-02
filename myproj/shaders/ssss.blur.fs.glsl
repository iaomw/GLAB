#version 460 core

uniform vec3 cam_position;
uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

uniform sampler2D gAlbedo; 
uniform sampler2D gExtra;

uniform int kernelSize;
uniform vec4 kernel[32];
//uniform vec4 weight[5];
//uniform vec4 variance[5];

uniform vec2 direction;
uniform float translucency;

uniform float ssss_width;
uniform vec3 ssss_falloff;
uniform vec3 ssss_strength;
uniform float luminance_threshold;
uniform float depthtest_coefficient;
//uniform float ssssMaxOffset;
//uniform float ssssTranslucency;

uniform float fovY;
uniform float farZ;
uniform float nearZ;
uniform float gamma;

layout (location = 0) out vec4 gColor;
//layout (location = 1) out vec4 gExtra;

in vec2 texCoords;

void main()
{   
    //vec3 cam_vspace = (myview_matrix * vec4(cam_position, 1.0)).xyz; 
    vec3 colorM = texture(gAlbedo, texCoords).rgb; 
    float depthM = texture(gAlbedo, texCoords).a;
    float mark = texture(gExtra, texCoords).a;
    // Ignore Background, hair, beard. 
    if (mark <= 0.0) { 
        gColor.rgba = texture(gAlbedo, texCoords); 
        return; //discard; 
    }

    if (texCoords.s > 0.5) {
        gColor.rgba = texture(gAlbedo, texCoords).rgba;
        return;
    } else if ((0.5 - texCoords.s) < 0.001) {
        gColor.rgba = vec4(0);
        return;
    }

    float luminance = (0.2126*colorM.r + 0.7152*colorM.g + 0.0722*colorM.b);
    if (luminance < luminance_threshold) {
        gColor.rgba = texture(gAlbedo, texCoords).rgba;
        return;
    }

    float SSSS_STREGTH_SOURCE = mark; //1.0;
    if (SSSS_STREGTH_SOURCE == 0.0) {
        gColor.rgba = texture(gAlbedo, texCoords); 
        return; 
    }

    vec2 imageSize = textureSize(gAlbedo, 0); 

    float distanceToCamera = 0.5 * imageSize.x / tan(0.5 * fovY); // pixel length
    float scale = distanceToCamera / abs(depthM); // pixel length / viewspace length

    // ssss_width is viewspace length
    // finalStep is texcoords space length 
    vec2 pixelStep = direction * ssss_width * scale;

    if (max(pixelStep.x, pixelStep.y) < 3) {
        //Step is smaller than 3 pixels
        gColor.rgba = texture(gAlbedo, texCoords); 
        return; 
    }

    // finalStep is texcoords space length 
    vec2 finalStep = pixelStep / imageSize.x;
    finalStep *= SSSS_STREGTH_SOURCE; // 
    finalStep *= 1.0 / 3.0; // Divide by 3 as the kernels range from -3 to 3.

    // Accumulate the center sample:
    vec3 colorBlurred = colorM;
    colorBlurred.rgb *= kernel[0].rgb;

    // Accumulate the other samples:
    for (int i = 1; i < kernelSize; i++) {
        // Fetch color and depth for current sample:
        vec2 offset = texCoords + kernel[i].a * finalStep;
        vec4 color = texture(gAlbedo, offset).rgba;

        // If the difference in depth is huge, we lerp color back to "colorM":
        float depth = texture(gAlbedo, offset).a; // view space depth
        //float s = SSSSSaturate(300.0f * distanceToProjectionWindow * sssWidth * abs(depthM - depth));
        float s = min(0.0125 * depthtest_coefficient * abs(depthM - depth), 1);
        //s *= 1 - color.a;
        if (color.a == 0) {s = 1;} 

        //color.rgb = SSSSLerp(color.rgb, colorM.rgb, s);
        color.rgb = mix(color.rgb, colorM.rgb, s);

        // Accumulate:
        colorBlurred.rgb += kernel[i].rgb * color.rgb;
    }

    gColor.rgb = colorBlurred.rgb;
    gColor.a = texture(gAlbedo, texCoords).a; 
}