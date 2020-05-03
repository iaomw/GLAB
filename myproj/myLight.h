#pragma once

#include <glm/glm.hpp>

#include "myShader.h"
#include "myTexture.h"

class myLight
{
public:
	enum LightType { POINTLIGHT, SPOTLIGHT, DIRECTIONALLIGHT };

	LightType type;

	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 intensity;
	glm::vec3 direction;
	myTexture* cubeShadowMap;

	glm::mat4 model_matrix;
	
	myLight();
	~myLight();

	myLight(LightType t, glm::vec3 p, glm::vec3 d);
	myLight(LightType t, glm::vec3 c, glm::vec3 p, glm::vec3 i, glm::vec3 d);
	
	void setUniform(myShader *shader, const std::string& lightvariable_in_shader);
};

