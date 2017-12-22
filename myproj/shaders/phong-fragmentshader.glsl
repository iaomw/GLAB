#version 330 core

uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 mymodel_matrix;

in vec4 myvertex;
in vec3 mynormal;

out vec4 color;

uniform struct Light 
{
	vec4 position;
	vec4 intensity;
	vec3 direction;
	int type;
} Lights[32];
uniform int num_lights;

uniform struct Material
{
	vec4 kd;
	vec4 ks;
	vec4 ka;
	float specular;
} material;

vec4 computeColor(vec4 _lightpos, vec4 lightcolor,
                  vec4 kd, vec4 ks, float specular_coefficient,
				  vec3 _normal, vec4 _mypos, vec4 _eyepos)
{
	vec3 mypos = _mypos.xyz / _mypos.w;
	vec3 eyepos = _eyepos.xyz / _eyepos.w;
	vec3 lightpos = _lightpos.xyz / _lightpos.w;
    vec3 normal = normalize(_normal);

    vec3 mypos_to_lightpos = normalize( lightpos - mypos );
	vec3 lightpos_to_mypos = -mypos_to_lightpos;
	vec3 mypos_to_eyepos = normalize(eyepos - mypos);

	vec3 reflection = normalize( lightpos_to_mypos + (2 * dot(mypos_to_lightpos, normal))*normal );

	vec4 mycolor = lightcolor * kd * max(dot(normal, mypos_to_lightpos), 0)
	             + lightcolor * ks * pow(max(dot(reflection, mypos_to_eyepos), 0), 20);

	return mycolor;
}

void main (void)
{   
	color = vec4(0,0,0,0);
	int i = 0;
	for (; i<num_lights;i++)
	    color += computeColor( Lights[i].position ,  Lights[i].intensity, 
                               material.kd, material.ks, material.specular,
     				           mynormal_matrix * mynormal, myview_matrix * mymodel_matrix * myvertex, vec4(0,0,0,1) );
	color.a = 1.0f;
}

