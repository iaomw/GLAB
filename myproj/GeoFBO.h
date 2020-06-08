#pragma once
#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "FBO.h"
#include "Texture.h"

typedef std::unordered_map<glm::vec3, std::map<Texture_Type, std::shared_ptr<Texture>>> PosTextureMap;

class GeoFBO: public FBO
{
public:

	std::shared_ptr<Texture> gPosition;
	std::shared_ptr<Texture> gNormal;
	std::shared_ptr<Texture> gAlbedo;

	GeoFBO();
	~GeoFBO();

	virtual void reset();

	void loopRender(std::shared_ptr<Shader> const& shader, std::unique_ptr<MeshPack> &object, glm::mat4 view_matrix, PosTextureMap& textureMap) {

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

