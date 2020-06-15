#version 330 core

uniform vec3 lookFrom;
uniform float farZ;

in vec4 world_pos;

void main()
{
    float lightDistance = length(world_pos.xyz - lookFrom);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / farZ;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}