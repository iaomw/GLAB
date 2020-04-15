#pragma once
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "FBO.h"
#include "myTexture.h"

class CubeFBO: public FBO
{
public:
	myTexture* envTexture;
	CubeFBO();
	~CubeFBO();

	void initFBO(const int& WIDTH, const int& HEIGHT, const GLenum format);

	void render(myShader* shader, myObject* object, 
				glm::mat4 captureViews[], glm::mat4 projection_matrix, unsigned int mipCount=1) {

		bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int maxMipLevels = mipCount;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = width * std::pow(0.5, mip);
			unsigned int mipHeight = height * std::pow(0.5, mip);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (maxMipLevels - 1);
			shader->setUniform("roughness", roughness);

			for (unsigned int i = 0; i < 6; ++i)
			{
				glNamedFramebufferTextureLayer(fboID, GL_COLOR_ATTACHMENT0, envTexture->texture_id, mip, i);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shader->setUniform("myview_matrix", captureViews[i]);
				shader->setUniform("myprojection_matrix", projection_matrix);

				object->displayObjects(shader, captureViews[i]);
			}
			if (mipCount <= 1) {
				glGenerateTextureMipmap(envTexture->texture_id);
			}

			auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
			}
		}

		shader->stop();
		unbind();
	}
};

