#pragma once

#include <glm/glm.hpp>
#include <string>

#include "myShader.h"

class myMaterial
{
public:
	myMaterial();
	myMaterial(glm::vec4 kd, glm::vec4 ks, glm::vec4 ka, float specular);
	myMaterial(myMaterial *);
	~myMaterial();

	void setUniform(myShader *shader, std::string name);
	
	glm::vec4 kd;
	glm::vec4 ks;
	glm::vec4 ka;
	float specular_coefficient;
	std::string mat_name;
};

