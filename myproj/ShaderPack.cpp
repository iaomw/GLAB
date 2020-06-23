#include <iostream>
#include "ShaderPack.h"

ShaderPack::ShaderPack()
{
	scene_buffer = std::make_unique<VBO>(GL_SHADER_STORAGE_BUFFER);
	scene_buffer->setData(&complex, sizeof(SceneComplex));

	pbr_pack_buffer = std::make_unique<VBO>(GL_SHADER_STORAGE_BUFFER);
	pbr_pack_buffer->setData(&pbr_pack, sizeof(PBR_Pack));

	pbr_pass_buffer = std::make_unique<VBO>(GL_SHADER_STORAGE_BUFFER);
	pbr_pass_buffer->setData(&pbr_pass, sizeof(PBR_Pass));

	light_pack_buffer = std::make_unique<VBO>(GL_SHADER_STORAGE_BUFFER);
	light_pack_buffer->setData(&light_pack, sizeof(Light_Pack));
}

ShaderPack::~ShaderPack()
{
}

void ShaderPack::syncSSBO() {

	scene_buffer->setData(&complex, sizeof(SceneComplex));

	pbr_pack_buffer->setData(&pbr_pack, sizeof(PBR_Pack));
	pbr_pass_buffer->setData(&pbr_pass, sizeof(PBR_Pass));
}

void ShaderPack::syncLight(std::vector<Light>& lightList) {

	size_t ele_count = lightList.size();

	size_t byte_size = ele_count * sizeof(Light);
	size_t byte_offset = offsetof(struct Light_Pack, list);

	size_t byte_range = byte_offset + byte_size;

	bool succ = light_pack_buffer->setData(lightList.data(), byte_offset, byte_size);

	if (!succ) {

		light_pack_buffer->setData(&light_pack, byte_range);
		light_pack_buffer->setData(lightList.data(), byte_offset, byte_size);
	}

	if (ele_count != light_pack.size) {

		light_pack.size = ele_count;
		light_pack_buffer->setData(&ele_count, 0, sizeof(GLuint));
	}
}

void ShaderPack::add(std::unique_ptr<Shader> shader, ShaderName type)
{
	if (shader == nullptr) return;

	GLuint block_index = 0;

	block_index = glGetProgramResourceIndex(shader->shaderprogram, GL_SHADER_STORAGE_BLOCK, "Light_Pack");

	if (block_index != -1) {

		GLuint ssbo_binding_point_index = 0;
		glShaderStorageBlockBinding(shader->shaderprogram, block_index, ssbo_binding_point_index);

		GLuint binding_point_index = 0;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, light_pack_buffer->getID());
	}

	block_index = glGetProgramResourceIndex(shader->shaderprogram, GL_SHADER_STORAGE_BLOCK, "SceneComplex");

	if (block_index != -1) {

		GLuint ssbo_binding_point_index = 1; // this number should be them same as below.
		// if binding index is already set in shader, this step could be ignored.
		glShaderStorageBlockBinding(shader->shaderprogram, block_index, ssbo_binding_point_index);

		GLuint binding_point_index = 1;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, scene_buffer->getID());
	}

	block_index = glGetProgramResourceIndex(shader->shaderprogram, GL_SHADER_STORAGE_BLOCK, "PBR_Pack");

	if (block_index != -1) {

		GLuint ssbo_binding_point_index = 2; 
		glShaderStorageBlockBinding(shader->shaderprogram, block_index, ssbo_binding_point_index);

		GLuint binding_point_index = 2;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, pbr_pack_buffer->getID());
	}

	block_index = glGetProgramResourceIndex(shader->shaderprogram, GL_SHADER_STORAGE_BLOCK, "PBR_Pass");

	if (block_index != -1) {

		GLuint ssbo_binding_point_index = 3;
		glShaderStorageBlockBinding(shader->shaderprogram, block_index, ssbo_binding_point_index);

		GLuint binding_point_index = 3;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, pbr_pass_buffer->getID());
	}
	
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
