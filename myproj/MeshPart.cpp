#include <math.h>
#include <iostream>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    

#include "Shader.h"
#include "MeshPart.h"

MeshPart::MeshPart(Material *m, size_t s, size_t e, const std::string& n = "noname")
{
	start = s;
	end = e;
	name = n;

	if (m != nullptr) 
		material = std::make_unique<Material>(m);
	else 
		material = std::make_unique<Material>();
	
	textures.clear();
}

MeshPart::MeshPart(size_t s, size_t e, const std::string& n) : MeshPart(nullptr, s, e, n) { }

void MeshPart::setTexture(Texture* tex, Texture_Type type)
{
	textures[type] = tex;
}

MeshPart::~MeshPart()
{
}

void MeshPart::display(std::unique_ptr<VAO> const &vao, std::unique_ptr<Shader> const &shader)
{
	if (end <= start) return;

	if (material != nullptr) material->setUniform(shader, "material");

	for (const auto& [type, tex] : textures) {

		if (nullptr == tex) { continue; }

		auto texName = magic_enum::enum_name(type);
		auto texIndex = magic_enum::enum_integer(type);

		shader->setTex(std::string(texName), tex->texture_id, texIndex);
	}
	
	if (vao != nullptr) vao->draw(start, end);
}
