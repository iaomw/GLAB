#pragma once

#include <chrono>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "FBO.h"
#include "myTexture.h"

class CubeFBO: public FBO
{
public:
	myTexture* envTexture;

	CubeFBO(bool shadow = false, GLenum textureFormat = GL_RGBA, GLenum internalFormat = GL_RGBA16F);
	virtual ~CubeFBO();

	void initFBO(const int& WIDTH, const int& HEIGHT);

	std::vector<glm::mat4> configCamera(glm::vec3 lookFrom) {

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
		std::vector<glm::mat4> captureViews =
		{
			glm::lookAt(lookFrom, lookFrom + glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(lookFrom, lookFrom + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(lookFrom, lookFrom + glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(lookFrom, lookFrom + glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(lookFrom, lookFrom + glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(lookFrom, lookFrom + glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		return captureViews;
	}

	void render(myShader* shader, myObject* object, 
				glm::vec3 lookFrom, glm::mat4 projection_matrix, unsigned int mipCount=1) {

		auto& captureViews = configCamera(lookFrom);

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

	void shadowMapping(myShader* shader, myObject* object, glm::vec3 lookFrom, glm::mat4 projection_matrix) {

		auto& captureViews = configCamera(lookFrom);		

		float near_plane = 0.1f;
		float far_plane = 1000.0f;

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

		bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = object->model_matrix;

		shader->setUniform("model", model);
		shader->setUniform("lightPos", lookFrom);
		shader->setUniform("far_plane", far_plane);
		
		for (size_t i = 0; i < 6; ++i) {
			shader->setUniform("shadowMatrices[" + std::to_string(i) + "]", captureProjection * captureViews[i]);
		}

		object->displayObjects(shader, captureViews[0]);

		shader->stop();
		unbind();
	}

private:
	bool shadow;
	GLenum textureFormat;
	GLenum internalFormat;
};

