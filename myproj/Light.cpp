#include "Light.h"
#include "Shader.h"

#include "default_constants.h"

Light::Light() : Light(LightType::POINTLIGHT, DEFAULT_LIGHT_POSITION, DEFAULT_LIGHT_DIRECTION)
{
	//type = LightType::POINTLIGHT;
	//position = DEFAULT_LIGHT_POSITION;
	//intensity = DEFAULT_LIGHT_INTENSITY;
	//direction = DEFAULT_LIGHT_DIRECTION;
}

Light::Light(LightType t, glm::vec3 p, glm::vec3 d) : Light(t, p, d, glm::vec3(1.0)) {}

Light::Light(LightType t, glm::vec3 p, glm::vec3 d, glm::vec3 c)
	:type(t), position(p), direction(d), color(c), intensity(1.0) {

	shadowFBO = std::make_unique<CubeFBO>(true);
	shadowFBO->initFBO(1024, 1024);
}

Light::~Light()
{
}

void Light::setUniform(std::shared_ptr<Shader> const& shader, const std::string& lightvariable_in_shader)
{
	auto type_i= magic_enum::enum_integer(type);
	shader->setUniform(lightvariable_in_shader + ".type", type_i);
	shader->setUniform(lightvariable_in_shader + ".color", color);
	shader->setUniform(lightvariable_in_shader + ".position", position);
	shader->setUniform(lightvariable_in_shader + ".direction", direction);
	shader->setUniform(lightvariable_in_shader + ".intensity", intensity);
}
