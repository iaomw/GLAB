#include "myLight.h"
#include "myShader.h"
#include "default_constants.h"


myLight::myLight()
{
	position = DEFAULT_LIGHT_POSITION;
	intensity = DEFAULT_LIGHT_INTENSITY;
	direction = DEFAULT_LIGHT_DIRECTION;
	type = POINTLIGHT;
}

myLight::myLight(glm::vec3 p, glm::vec3 c, LightType t)
	: position(p), intensity(c), type(t)
{
}


myLight::~myLight()
{
}

void myLight::setUniform(myShader* shader, std::string lightvariable_in_shader) 
{
	shader->setUniform(lightvariable_in_shader + ".position", position);
	shader->setUniform(lightvariable_in_shader + ".intensity", intensity);
	shader->setUniform(lightvariable_in_shader + ".direction", direction);
	shader->setUniform(lightvariable_in_shader + ".type", type);
}
