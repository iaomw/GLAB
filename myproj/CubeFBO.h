#pragma once

#include "FBO.h"

#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CubeFBO: public FBO
{
public:
	std::unique_ptr<Texture> envTexture;

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

	void render(std::unique_ptr<Shader> const& shader, std::unique_ptr<MeshPack> const &object,
				glm::vec3 lookFrom, glm::mat4 projection_matrix, unsigned int mipCount=1) {

		auto& captureViews = configCamera(lookFrom);

		this->bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int maxMipLevels = mipCount;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			GLsizei mipWidth = width * std::pow(0.5, mip);
			GLsizei mipHeight = height * std::pow(0.5, mip);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (maxMipLevels - 1);
			shader->setUniform("roughness", roughness);

			for (unsigned int i = 0; i < 6; ++i)
			{
				glNamedFramebufferTextureLayer(fboID, GL_COLOR_ATTACHMENT0, envTexture->texture_id, mip, i);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shader->setUniform("view_matrix", captureViews[i]);
				shader->setUniform("projection_matrix", projection_matrix);

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
		this->unbind();
	}

	void shadowMapping(std::unique_ptr<Shader> const &shader, std::unique_ptr<MeshPack> const &object, glm::vec3 &lookFrom, glm::mat4 &projection_matrix) {

		auto& captureViews = configCamera(lookFrom);		

		float near_plane = 0.0f;
		float far_plane = 1000.0f;

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

		this->bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4& model = object->model_matrix;

		shader->setUniform("model", model);
		shader->setUniform("lightPos", lookFrom);
		shader->setUniform("far_plane", far_plane);
		
		for (size_t i = 0; i < 6; ++i) {
			shader->setUniform("shadowMatrices[" + std::to_string(i) + "]", captureProjection * captureViews[i]);
		}

		object->displayObjects(shader, captureViews[0]);

		shader->stop();
		this->unbind();
	}

private:
	bool shadow;
	GLenum textureFormat;
	GLenum internalFormat;
};

