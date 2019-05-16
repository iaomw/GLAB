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
    gColor = texture(cubetex, texCoord);
    gExtra = myview_matrix * mymodel_matrix * myvertex; 
}