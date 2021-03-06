#pragma once

#include <memory>
#include <unordered_map>
#include "magic_enum.hpp"

#include "VBO.h"
#include "Light.h"
#include "Shader.h"

#include <GL/glew.h>

enum class ShaderName {

	geo_buffer, pbr_buffer, postprocess,

	skycube, basic, blur, point_shadow,

	equirectangular, irradiance, prefilter, brdf,

	ssss_phong, ssss_blur
};

static std::string literial(ShaderName name) {
	return std::string(magic_enum::enum_name(name));
}

struct alignas(16) SceneComplex {
//struct SceneComplex {
	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;
	glm::mat4 weiv_matrix;

	float nearZ;
	float farZ;
	float fovY; 
	float XdY;

	float exposure;
	float gamma;
};

struct Light_Pack {
	GLuint lightCount = 3;
	Light lightList[3];
};

struct PBR_Unit {
	GLuint64 ao;
	GLuint64 base;
	GLuint64 metalness;
	GLuint64 normal;
	GLuint64 roughness;
};

struct PBR_Pack {
	
	GLuint size = 3; 
	PBR_Unit list[3];
};

struct alignas(16) PBR_Pass {
	
	GLuint64 irradiance;
	GLuint64 prefilter;
	GLuint64 brdf;

	GLuint64 complex;
	GLuint64 albedo;
};

class ShaderPack
{
private:

	std::unique_ptr<VBO> scene_buffer;

	std::unique_ptr<VBO> pbr_pack_buffer;
	std::unique_ptr<VBO> pbr_pass_buffer;

	std::unique_ptr<VBO> light_pack_buffer;

	std::unordered_map<ShaderName, size_t> byname;
	std::vector<std::unique_ptr<Shader>> all_shaders;

public:

	SceneComplex complex;
	Light_Pack light_pack;

	PBR_Pack pbr_pack;
	PBR_Pass pbr_pass;
	
	ShaderPack();
	~ShaderPack();

	void syncSSBO();

	void add(std::unique_ptr<Shader> shader, ShaderName type);
	std::unique_ptr<Shader> const& operator[](ShaderName type); //const
	std::unique_ptr<Shader> const& operator[](const unsigned int index); //const

	size_t size() const;
};

