#include "myLight.h"
#include "myShader.h"

#include "default_constants.h"

myLight::myLight()
{
	type = LightType::POINTLIGHT;
	position = DEFAULT_LIGHT_POSITION;
	intensity = DEFAULT_LIGHT_INTENSITY;
	direction = DEFAULT_LIGHT_DIRECTION;

	shadowFBO = new CubeFBO(true); 
	shadowFBO->initFBO(1024, 1024);
}

myLight::myLight(LightType t, glm::vec3 p, glm::vec3 d)
	:type(t), color(1.0), position(p), direction(d), intensity(1.0) {
	shadowFBO = new CubeFBO(true);
	shadowFBO->initFBO(1024, 1024);
}

myLight::myLight(LightType t, glm::vec3 c, glm::vec3 p, glm::vec3 d, glm::vec3 i)
	:type(t), color(c), position(p), direction(d), intensity(i) {
	shadowFBO = new CubeFBO(true);
	shadowFBO->initFBO(1024, 1024);
}

myLight::~myLight()
{
	delete shadowFBO;
}

void myLight::setUniform(myShader* shader, const std::string& lightvariable_in_shader) 
{
	auto type_i= magic_enum::enum_integer(type);
	shader->setUniform(lightvariable_in_shader + ".type", type_i);
	shader->setUniform(lightvariable_in_shader + ".color", color);
	shader->setUniform(lightvariable_in_shader + ".position", position);
	shader->setUniform(lightvariable_in_shader + ".direction", direction);
	shader->setUniform(lightvariable_in_shader + ".intensity", intensity);
}
