#pragma once
#include "Light.h"
#include "Shader.h"

#include <vector>


class LightList
{
public:
	std::vector<std::unique_ptr<Light>> list;

	LightList();
	~LightList();

	void addLight(std::unique_ptr<Light> light);

	void setUniform(std::shared_ptr<Shader> const& shader, const std::string& lightvariable_in_shader);
};

