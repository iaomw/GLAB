#pragma once

#include "Light.h"
#include <vector>
#include <memory>

class LightList
{
public:
	std::vector<std::unique_ptr<Light>> list;

	void addLight(std::unique_ptr<Light> light);

	LightList();
	~LightList();
};

