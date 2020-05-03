#pragma once
#include "myLight.h"
#include "myShader.h"

#include <vector>


class myLights
{
public:
	std::vector<myLight *> lights;

	myLights();
	~myLights();

	void addLight(myLight *);

	void setUniform(myShader *shader, const std::string& lightvariable_in_shader);
};

