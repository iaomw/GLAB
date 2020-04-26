#version 460 core

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 myprojection_matrix;

uniform sampler2D gAlbedo; 
uniform sampler2D gExtra;

uniform vec2 direction;

uniform int kernelSize;
uniform vec4 kernel[32];
//uniform vec4 weight[5];
//uniform vec4 variance[5];

uniform float ssssWidth;
uniform float ssssStrength;
uniform float depthTestCoefficient;
//uniform float ssssMaxOffset;
//uniform float ssssTranslucency;

uniform float fovY;
uniform float farZ;
uniform float nearZ;

uniform float gamma;

uniform vec3 cam_position;

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
    if (mark == 0.0) { 
        gColor.rgba = texture(gAlbedo, texCoords); 
        return; //discard; 
    }
    //colorM = pow(colorM, vec3(1.0/gamma));

    //float luminance = (0.2126*colorM.r + 0.7152*colorM.g + 0.0722*colorM.b);
    //if (luminance < 0.3) {discard;}

    float SSSS_STREGTH_SOURCE = ssssStrength; //1.0;
    if (SSSS_STREGTH_SOURCE==0.0)  discard;

    float distanceToCamera = 1.0 / tan(0.5 * fovY);
    float scale = distanceToCamera / abs(depthM);

    // Calculate the final step to fetch the surrounding pixels:
    vec2 finalStep = direction * ssssWidth * scale;
    finalStep *= SSSS_STREGTH_SOURCE; // 
    finalStep *= 1.0 / 3.0; // Divide by 3 as the kernels range from -3 to 3.

    // Accumulate the center sample:
    vec3 colorBlurred = colorM;
    colorBlurred.rgb *= kernel[0].rgb;

    // Accumulate the other samples:
    for (int i = 1; i < kernelSize; i++) {
        // Fetch color and depth for current sample:
        vec2 offset = texCoords + kernel[i].a * finalStep;
        vec3 color = texture(gAlbedo, offset).rgb;
        //color = pow(color, vec3(1.0/gamma));
        //color.rgb *= 1.0; //vec3(color.a);

        //#if SSSS_FOLLOW_SURFACE == 1
        // If the difference in depth is huge, we lerp color back to "colorM":
        float depth = texture(gAlbedo, offset).a;
        //float s = SSSSSaturate(300.0f * distanceToProjectionWindow * sssWidth * abs(depthM - depth));
        //float s = 300.0f * distanceToCamera * ssssWidth * abs(depthM - depth);

        //float s = depthTestCoefficient * distanceToCamera * ssssWidth * abs(depthM - depth);
        float s =  0.0125 * ssssWidth * abs(depthM - depth);
        s = clamp(s, 0.0, 1.0);

        //color.rgb = SSSSLerp(color.rgb, colorM.rgb, s);
        color.rgb = mix(color.rgb, colorM.rgb, s);
        //#endifs

        // Accumulate:
        colorBlurred.rgb += kernel[i].rgb * color.rgb;
    }

    //gColor.rgb = pow(colorBlurred.rgb, vec3(gamma));
    gColor.rgb = colorBlurred.rgb;
    gColor.a = texture(gAlbedo, texCoords).a; 

    if (texCoords.s > 0.5) {
        gColor.rgb = texture(gAlbedo, texCoords).rgb;
    } else if ((0.5 - texCoords.s) < 0.001) {
        gColor.rgb = vec3(0);
    }
}