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

typedef std::unordered_map<glm::vec3, std::map<TEXTURE_TYPE, myTexture*>, KeyFuncs, KeyFuncs> PosTextureMap;

class GeoFBO: public FBO
{
public:

	myTexture* gPosition;
	myTexture* gNormal;
	myTexture* gAlbedo;

	GeoFBO();
	~GeoFBO();

	void loopRender(myShader* shader, myObject* object, glm::mat4 view_matrix, PosTextureMap& textureMap) {

		bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (auto& [trans, texMap] : textureMap) {
			
			object->setTexture(texMap[TEXTURE_TYPE::texAlbedo], TEXTURE_TYPE::texAlbedo);
			object->setTexture(texMap[TEXTURE_TYPE::texAO], TEXTURE_TYPE::texAO);
			object->setTexture(texMap[TEXTURE_TYPE::texMetal], TEXTURE_TYPE::texMetal);
			object->setTexture(texMap[TEXTURE_TYPE::texNormal], TEXTURE_TYPE::texNormal);
			object->setTexture(texMap[TEXTURE_TYPE::texRough], TEXTURE_TYPE::texRough);

			object->translate(trans.x, trans.y, trans.z);
			object->displayObjects(shader, view_matrix);
			object->translate(-trans.x, -trans.y, -trans.z);
		};

		shader->stop();
		unbind();
	}

	void initFBO(int WIDTH, int HEIGHT);
};

