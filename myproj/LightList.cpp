#include "LightList.h"
#include <string>

LightList::LightList()
{
}


LightList::~LightList()
{
}

void LightList::addLight(std::unique_ptr<Light> light)
{
	list.push_back(std::move(light));
}

void LightList::setUniform(std::shared_ptr<Shader> const& shader, const std::string& lightvariable_in_shader)
{
	for (unsigned int i = 0; i < list.size(); ++i) {

		auto shaderString = lightvariable_in_shader + "[" + std::to_string(i) + "]";
		
		list[i]->setUniform(shader, shaderString);
	}

	shader->setUniform("num_lights", static_cast<int>(list.size()));
}
