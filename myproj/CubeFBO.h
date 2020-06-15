#pragma once

#include "FBO.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CubeFBO: public FBO
{
private:
	bool shadow;
	
	GLenum textureFormat;
	GLenum internalFormat;

	const int mipmap_level = 6;

	const float nearZ = 0.0f;
	const float farZ = 1000.0f;

	const glm::mat4& captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearZ, farZ);

public:
	std::unique_ptr<Texture> envTexture;

	CubeFBO(bool shadow = false, GLenum textureFormat = GL_RGBA, GLenum internalFormat = GL_RGBA16F);
	virtual ~CubeFBO();

	void initFBO(const int& WIDTH, const int& HEIGHT);

	std::vector<glm::mat4> configCamera(glm::vec3 lookFrom) {

		std::vector<glm::mat4> captureViews
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
				glm::vec3 lookFrom, glm::mat4 projection_matrix, const bool custom_mipmap=false) {

		auto& captureViews = configCamera(lookFrom);

		this->bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->setUniform("farZ", farZ);
		shader->setUniform("lookFrom", lookFrom);
		shader->setUniform("projection_matrix", projection_matrix);

		unsigned int max_level = custom_mipmap? mipmap_level:1;
		for (unsigned int mip = 0; mip < max_level; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			GLsizei mipWidth = width * std::pow(0.5, mip);
			GLsizei mipHeight = height * std::pow(0.5, mip);
			glViewport(0, 0, mipWidth, mipHeight);

			if (max_level > 1) {
				float roughness = (float)mip / (max_level - 1);
				shader->setUniform("roughness", roughness);
			}

			for (unsigned int i = 0; i < 6; ++i)
			{
				glNamedFramebufferTextureLayer(fboID, GL_COLOR_ATTACHMENT0, envTexture->texture_id, mip, i);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				object->displayObjects(shader, captureViews[i]);
			}
		}

		if (!custom_mipmap) {
			glGenerateTextureMipmap(envTexture->texture_id);
		}

		auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
		}

		shader->stop();
		this->unbind();
	}

	void shadowMapping(std::unique_ptr<Shader> const &shader, std::unique_ptr<MeshPack> const &object, glm::vec3 &lookFrom, glm::mat4 &projection_matrix) {

		auto& captureViews = configCamera(lookFrom);		

		this->bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->setUniform("farZ", farZ);
		shader->setUniform("lookFrom", lookFrom);
		
		for (size_t i = 0; i < 6; ++i) {
			shader->setUniform("shadowMatrices[" + std::to_string(i) + "]", captureProjection * captureViews[i]);
		}

		object->displayObjects(shader, captureViews[0]);

		shader->stop();
		this->unbind();
	}
};

