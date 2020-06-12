#include <iostream>
#include "ShaderPack.h"

ShaderPack::ShaderPack()
{
}

ShaderPack::~ShaderPack()
{
}

void ShaderPack::add(std::unique_ptr<Shader> shader, ShaderName type)
{
	if (shader == nullptr) return;

	byname[type] = all_shaders.size();
	all_shaders.push_back(std::move(shader));
}

std::unique_ptr<Shader> const& ShaderPack::operator[](ShaderName type)
{
	assert(byname.count(type) != 0);
	
	return all_shaders[byname[type]];
}

std::unique_ptr<Shader> const& ShaderPack::operator[](const unsigned int index)
{
	assert(index >= 0 && index < size());

	return all_shaders[index];
}

size_t ShaderPack::size() const
{
	return all_shaders.size();
}
