#pragma once
#include <GL/glew.h>

#include "FBO.h"
#include "myTexture.h"

struct KeyFuncs
{
	size_t operator()(const glm::vec3& k)const
	{
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
	}

	bool operator()(const glm::vec3& a, const glm::vec3& b)const
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
};

typedef std::unordered_map<glm::vec3, std::map<Texture_Type, myTexture*>, KeyFuncs, KeyFuncs> PosTextureMap;

class GeoFBO: public FBO
{
public:

	myTexture* gPosition;
	myTexture* gNormal;
	myTexture* gAlbedo;

	GeoFBO();
	~GeoFBO();

	virtual void reset();

	void loopRender(myShader* shader, myObject* object, glm::mat4 view_matrix, PosTextureMap& textureMap) {

		bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (auto& [trans, texMap] : textureMap) {
			
			object->setTexture(texMap[Texture_Type::texAlbedo], Texture_Type::texAlbedo);
			object->setTexture(texMap[Texture_Type::texAO], Texture_Type::texAO);
			object->setTexture(texMap[Texture_Type::texMetal], Texture_Type::texMetal);
			object->setTexture(texMap[Texture_Type::texNormal], Texture_Type::texNormal);
			object->setTexture(texMap[Texture_Type::texRough], Texture_Type::texRough);

			object->translate(trans.x, trans.y, trans.z);
			object->displayObjects(shader, view_matrix);
			object->translate(-trans.x, -trans.y, -trans.z);
		};

		shader->stop();
		unbind();
	}

	void initFBO(int WIDTH, int HEIGHT);
};

