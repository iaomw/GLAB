#pragma once

#include <GL/glew.h>
#define GLM_FORCE_AVX
#include <glm/glm.hpp>

enum class LightType { POINTLIGHT, SPOTLIGHT, DIRECTIONALLIGHT };

class alignas(16) Light
//class Light
{
public:

	GLuint64 type;
	GLuint64 shadow_handle;
	
	glm::vec3 color;      float __paddingA;
	glm::vec3 position;   float __paddingB;
	glm::vec3 intensity;  float __paddingC;
	glm::vec3 direction;  float __paddingD;

	glm::mat4 model_matrix;
	
	Light();
	~Light();

	Light(LightType t, glm::vec3 p, glm::vec3 d);
	Light(LightType t, glm::vec3 p, glm::vec3 d, glm::vec3 c);
};

