#pragma once

#include <vector>
#include <string>
#include <map>

#include "magic_enum.hpp"
#include <glm/glm.hpp>

#include "myVAO.h"
#include "myTexture.h"
#include "myMaterial.h"


class myShader;

enum class Texture_Type {

	colortex, bumptex, cubetex, depthtex,

	texAO, texAlbedo, texMetal, texNormal, texRough,

	gPosition, gAlbedo, gNormal, gEnv,

	gIrradiance, gPrefilter, BRDF_LUT,

	gExtra, gBloom, gSSSS, 
	
	shadowCube, shadowDir,

	Unknow
};


class mySubObject
{
public:
	size_t start, end;
	myMaterial *material;

	std::map<Texture_Type, myTexture *> textures;

	std::string name;

	mySubObject(myMaterial *m, size_t s, size_t e, const std::string& n) ;
	mySubObject(size_t s, size_t e, const std::string&);
	
	~mySubObject();

	void setTexture(myTexture *t, Texture_Type);

	void displaySubObject(myVAO*, myShader*);
	void displayNormals(myVAO*, myShader*);
};
