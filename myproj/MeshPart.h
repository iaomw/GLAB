#pragma once

#include <map>
#include <vector>
#include <string>

#define GLM_FORCE_AVX
#include <glm/glm.hpp>
#include "magic_enum.hpp"

#include "VAO.h"
#include "Texture.h"
#include "Material.h"

class MeshPart
{
public:
	size_t start, end;
	std::unique_ptr<Material> material;

	std::map<Texture_Type, Texture*> textures;

	std::string name;

	MeshPart(size_t s, size_t e, const std::string& n);
	MeshPart(Material *m, size_t s, size_t e, const std::string& n) ;
	
	~MeshPart();

	void setTexture(Texture* tex, Texture_Type type);

	void display(std::unique_ptr<VAO> const &vao, std::unique_ptr<Shader> const &shader);

	void display(std::unique_ptr<VAO> const& vao, std::unique_ptr<Shader> const& shader, 
		std::vector<glm::mat4>& modelMatrixList, std::vector<glm::mat4>& normalMatrixList);
};
