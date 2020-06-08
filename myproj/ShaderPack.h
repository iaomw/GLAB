#pragma once

#include <unordered_map>
#include "Shader.h"

class ShaderPack
{
public:
	std::unordered_map<std::string, size_t> byname;
	std::vector<std::shared_ptr<Shader>> all_shaders;

	ShaderPack();
	~ShaderPack();

	void add(std::shared_ptr<Shader> const& shader, const std::string& name);
	std::shared_ptr<Shader> const& operator[](const std::string& name);
	std::shared_ptr<Shader> const& operator[](const unsigned int index);

	size_t size() const;
};

