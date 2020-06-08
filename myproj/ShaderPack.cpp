#include <iostream>
#include "ShaderPack.h"

ShaderPack::ShaderPack()
{
}

ShaderPack::~ShaderPack()
{
}

void ShaderPack::add(std::shared_ptr<Shader> const& shader, const std::string& name)
{
	if (shader == nullptr) return;

	byname[name] = all_shaders.size();
	all_shaders.push_back(shader);
}

std::shared_ptr<Shader> const& ShaderPack::operator[](const std::string& name)
{
	assert(byname.count(name) != 0);
	
	return all_shaders[byname[name]];
}

std::shared_ptr<Shader> const& ShaderPack::operator[](const unsigned int index)
{
	assert(index >= 0 && index < size());

	return all_shaders[index];
}

size_t ShaderPack::size() const
{
	return all_shaders.size();
}
