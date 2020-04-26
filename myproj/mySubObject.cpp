#include <math.h>
#include <iostream>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    

#include "mySubObject.h"
#include "myShader.h"
#include "myLight.h"

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

void mySubObject::setTexture(myTexture* t, Texture_Type type)
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

	for (const auto& [type, tex] : textures) {

		if (tex == nullptr) { continue; }

		auto texName = magic_enum::enum_name(type);
		auto texIndex = magic_enum::enum_integer(type);
		tex->bind(shader, texName, texIndex);
	}
	
	if (vao != nullptr) vao->draw(start, end);
}

void mySubObject::displayNormals(myVAO* vao, myShader*)
{
}


