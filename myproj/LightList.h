#pragma once
#include <vector>
#include <memory>

#include "Light.h"
#include "CubeFBO.h"

class LightList
{
public:
	std::vector<Light> list;
	std::vector<std::unique_ptr<CubeFBO>> shadowList;

	void addLight(Light light);

	LightList();
	~LightList();
};

