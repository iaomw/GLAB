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

myLight::myLight(LightType t, glm::vec3 p, glm::vec3 d)
	:type(t), color(1.0), position(p), direction(d), intensity(1.0) {
}

myLight::myLight(LightType t, glm::vec3 c, glm::vec3 p, glm::vec3 d, glm::vec3 i)
	:type(t), color(c), position(p), direction(d), intensity(i) {
}

myLight::~myLight()
{
}

void myLight::setUniform(myShader* shader, std::string lightvariable_in_shader) 
{
	shader->setUniform(lightvariable_in_shader + ".type", type);
	shader->setUniform(lightvariable_in_shader + ".color", color);
	shader->setUniform(lightvariable_in_shader + ".position", position);
	shader->setUniform(lightvariable_in_shader + ".direction", direction);
	shader->setUniform(lightvariable_in_shader + ".intensity", intensity);
}
