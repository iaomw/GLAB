#include <math.h>
#include <iostream>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    

#include "mySubObject.h"
#include "myShader.h"

using namespace std;

mySubObject::mySubObject(myMaterial *m, size_t s, size_t e, string n = "noname")
{
	start = s;
	end = e;
	name = n;

	if (m != nullptr) material = new myMaterial(m);
	else material = new myMaterial();
	
	textures.clear();
}

mySubObject::mySubObject(size_t s, size_t e, string n) : mySubObject(nullptr, s, e, n) { }

void mySubObject::setTexture(myTexture* t, TEXTURE_TYPE type)
{
	textures[type] = t;
}

mySubObject::~mySubObject()
{

}

void mySubObject::displaySubObject(myVAO *vao, myShader *shader)
{
	if (end <= start) return;

	if (material != nullptr) material->setUniform(shader, "material");

	if (textures.find(COLORMAP) != textures.end() && textures[COLORMAP] != nullptr)
	{
		shader->setUniform("totexture", 1);
		textures[COLORMAP]->bind(shader, "tex", 1);
	}
	else
	{
		shader->setUniform("tex", 11);
		shader->setUniform("totexture", 0);
	}

	if (textures.find(BUMPMAP) != textures.end() && textures[BUMPMAP] != nullptr )
		textures[BUMPMAP]->bind(shader, "bumptex", 2);
	else 
		shader->setUniform("bumptex", static_cast<int>(11));

	if (textures.find(CUBEMAP) != textures.end() && textures[CUBEMAP] != nullptr)
		textures[CUBEMAP]->bind(shader, "cubetex", 3);
	else 
		shader->setUniform("cubetex", static_cast<int>(11));

	std::map<TEXTURE_TYPE, string> textNameMap = { 
		{COLORMAP, "" },
		{BUMPMAP, "" },
		{CUBEMAP, ""},

		{pAlbedo, "texAlbedo"},
		{pAO, "texAO"},
		{pHeight, "texHeight"},
		{pMetal, "texMetal"},
		{pNormal, "texNormal"},
		{pRough, "texRough"},

		{gPosition, "gPosition"},
		{gAlbedo, "gAlbedo"},
		{gNormal, "gNormal"},
		{gEnv, "gEnv"},

		{Unknow, ""}
	};

	for (int texType = pAlbedo; texType != Unknow; ++texType)
	{
		auto foo = static_cast<TEXTURE_TYPE>(texType);
		auto texName = textNameMap[foo];
		auto texIndex = texType + 1;

		if (textures.find(foo) != textures.end() && textures[foo] != nullptr)
			textures[foo]->bind(shader, texName, texIndex);
		else
			shader->setUniform(texName, static_cast<int>(11));
	}
	
	if (vao != nullptr) vao->draw(start, end);
}

void mySubObject::displayNormals(myVAO* vao, myShader*)
{
}


