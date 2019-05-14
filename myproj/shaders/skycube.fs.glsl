#version 330 core
out vec4 FragColor;
in vec3 texCoords;

layout (location = 0) out vec4 gColor;

uniform samplerCube cubetex;

void main()
{    
    gColor = texture(cubetex, texCoords);
}