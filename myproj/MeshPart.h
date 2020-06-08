#pragma once

#include <vector>
#include <string>
#include <map>

#include "magic_enum.hpp"
#include <glm/glm.hpp>

#include "VAO.h"
#include "Texture.h"
#include "Material.h"

enum class Texture_Type {

	colortex, bumptex, cubetex, depthtex,

	texAO, texAlbedo, texMetal, texNormal, texRough,

	gPosition, gAlbedo, gNormal, gEnv,

	gIrradiance, gPrefilter, BRDF_LUT,

	gExtra, gBloom, gSSSS, 
	
	shadowCube, shadowDir,

	Unknow
};

class MeshPart
{
public:
	size_t start, end;
	std::unique_ptr<Material> material;

	std::map<Texture_Type, std::shared_ptr<Texture>> textures;

	std::string name;

	MeshPart(size_t s, size_t e, const std::string& n);
	MeshPart(Material *m, size_t s, size_t e, const std::string& n) ;
	
	~MeshPart();

	void setTexture(std::shared_ptr<Texture> const& tex, Texture_Type);

	void display(std::unique_ptr<VAO> const &vao, std::shared_ptr<Shader> const &shader);
};
