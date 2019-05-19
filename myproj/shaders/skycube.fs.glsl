#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

in vec4 myvertex;
in vec3 mynormal;
in vec3 texCoord;

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;

uniform samplerCube cubetex;

void main()
{    
    gColor = texture(cubetex, texCoord);gColor.a = 0.0;
    gExtra = myview_matrix * mymodel_matrix * myvertex; 

    //float brightness = dot(gColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    //if(brightness > 1.0)
      //   gColor.w = 1.0;
    //else    
        // gColor.w = 0.0;
}