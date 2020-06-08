#pragma once

#include <glm/glm.hpp>

#include "CubeFBO.h"
#include "Shader.h"
#include "Texture.h"
#include "magic_enum.hpp"

enum class LightType { POINTLIGHT, SPOTLIGHT, DIRECTIONALLIGHT };

class Light
{
public:

	LightType type;

	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 intensity;
	glm::vec3 direction;

	glm::mat4 model_matrix;
	std::unique_ptr<CubeFBO> shadowFBO;
	
	Light();
	~Light();

	Light(LightType t, glm::vec3 p, glm::vec3 d);
	Light(LightType t, glm::vec3 p, glm::vec3 d, glm::vec3 c);
	
	void setUniform(std::shared_ptr<Shader> const& shader, const std::string& lightvariable_in_shader);
};

