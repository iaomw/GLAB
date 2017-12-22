#pragma once

#include <glm/glm.hpp>
#include "myShader.h"

class myLight
{
public:
	enum LightType { POINTLIGHT, SPOTLIGHT, DIRECTIONALLIGHT };

	glm::vec3 position;
	glm::vec3 intensity;
	glm::vec3 direction;
	LightType type;

	glm::mat4 model_matrix;
	
	myLight();
	myLight(glm::vec3 p, glm::vec3 c, LightType t);
	~myLight();

	void setUniform(myShader *shader, std::string lightvariable_in_shader);
};

