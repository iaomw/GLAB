#pragma once

#include <memory>
#include <unordered_map>
#include "magic_enum.hpp"

#include "Shader.h"

enum class ShaderName {

	geo_buffer, pbr_buffer, postprocess,

	skycube, basic, blur, point_shadow,

	equirectangular, irradiance, prefilter, brdf,

	ssss_phong, ssss_blur
};

static std::string literial(ShaderName name) {
	return std::string(magic_enum::enum_name(name));
}

class ShaderPack
{
private:
	std::unordered_map<ShaderName, size_t> byname;
	std::vector<std::unique_ptr<Shader>> all_shaders;

public:
	
	ShaderPack();
	~ShaderPack();

	void add(std::unique_ptr<Shader> shader, ShaderName type);
	std::unique_ptr<Shader> const& operator[](ShaderName type); //const
	std::unique_ptr<Shader> const& operator[](const unsigned int index); //const

	size_t size() const;
};

