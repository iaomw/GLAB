#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class Material
{
public:
	Material();
	~Material();

	Material(Material*);
	Material(glm::vec4 kd, glm::vec4 ks, glm::vec4 ka, float specular);
	
	void setUniform(std::unique_ptr<Shader> const& shader, const std::string& name);
	
	glm::vec4 kd;
	glm::vec4 ks;
	glm::vec4 ka;
	float specular_coefficient;
	std::string mat_name;
};

