#version 450 core

uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;

const float PI  = 3.14159265358979323846264338327950288419716939937510f;

in vec4 myvertex;
in vec3 mynormal;
in vec2 texCoord;

in vec4 vertex_viewspace;
in vec4 normal_viewspace;

uniform float fovY;
uniform float farZ;
uniform float nearZ;

uniform vec3 cam_position;

uniform int totexture;
uniform sampler2D tex;
uniform sampler2D bumptex;

struct Light {
    int type;
    vec3 color;
    vec3 position;
    vec3 intensity;	
    vec3 direction;
};

uniform int num_lights;
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gExtra;

float saturate(float f)
{
    return clamp(f, 0.0f, 1.0f);
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * nearZ * farZ) / (farZ + nearZ - z * (farZ - nearZ));
}

// Calculation in view space
vec3 detailedTexNormal(vec3 normal_vspace, vec3 tNormal, vec3 position)
{ 
    vec3 dPosX  = dFdx(position.xyz);
    vec3 dPosY  = dFdy(position.xyz);
    vec2 dTexX = dFdx(texCoord);
    vec2 dTexY = dFdy(texCoord);

    vec3 normal = normalize(normal_vspace);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * tNormal);
}

void main (void)
{   
	vec3 tNormal = texture(bumptex, texCoord.st).rgb; 
    tNormal.x = -tNormal.x;
    //tNormal.y = -tNormal.y;
    tNormal.z = -tNormal.z;
   
    vec4 normal_vs = myview_matrix * mymodel_matrix * vec4(mynormal, 1.0);

	vec3 normal = detailedTexNormal(normal_vs.xyz, tNormal, vertex_viewspace.xyz);

	vec4 color = vec4(0.0); vec4 in_kd = vec4(0.0);

	vec3 cam_vspace = (myview_matrix * vec4(cam_position, 1.0)).xyz; 

    vec3 V = normalize(cam_vspace - vertex_viewspace.xyz);
    vec3 N = normalize(normal);
    vec3 R = reflect(-V, N);

	color = texture(tex, texCoord.st);
	color.rgb = pow(color.rgb, vec3(2.2));
    
	for (int i=0; i<num_lights; i++)
	{
		vec4 light_pos = myview_matrix * vec4(lights[i].position, 1.0); // world space to view space
        vec3 direction = (light_pos-vertex_viewspace).xyz;
        vec3 L = normalize(direction);
        vec3 H = normalize(L + V);

		vec3 lightColor = lights[i].color; 
        vec3 intensity = lights[i].intensity;

		float NdotL = saturate(dot(N, L));
		float NdotH = saturate(dot(N, H));

        vec3 diffuse = lightColor * NdotL / PI;

		in_kd.rgb += lightColor * pow(NdotH, 1024.0f);
		//in_kd.rgb += diffuse;
		in_kd.rgb *= intensity;
	} 

	color.rgb += in_kd.rgb;
	color.a = 1.0f;

	gColor = color;
	
    gExtra.xyz = vertex_viewspace.rgb;
	gExtra.w = LinearizeDepth(gl_FragCoord.z);
}

