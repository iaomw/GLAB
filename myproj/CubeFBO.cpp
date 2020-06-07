#include "CubeFBO.h"
#include <iostream>

CubeFBO::CubeFBO(bool shadow, GLenum textureFormat, GLenum internalFormat) : FBO()
{
	this->shadow = shadow;
	envTexture = nullptr; fboID = 0;
	this->textureFormat = textureFormat;
	this->internalFormat = internalFormat;
}

CubeFBO::~CubeFBO()
{
	if (fboID != 0) {
		glDeleteFramebuffers(1, &fboID); 
		fboID = 0;
	}
	delete envTexture;
}

void CubeFBO::initFBO(const int& WIDTH, const int& HEIGHT)
{
	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	glCreateFramebuffers(1, &fboID);
	width = WIDTH; height = HEIGHT;

	delete envTexture;
	envTexture = new myTexture(GL_TEXTURE_CUBE_MAP);
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &(envTexture->texture_id));

	if (shadow) {

		envTexture->textureFormat = GL_DEPTH_COMPONENT;
		envTexture->width = width; envTexture->height = height;

		//glTextureStorage2D(envTexture->texture_id, 1, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT);

		glBindTexture(GL_TEXTURE_CUBE_MAP, envTexture->texture_id);
		for (size_t face = 0; face < 6; ++face)
		{
			//glTextureSubImage3D(envTexture->texture_id, 0, 0, 0, face, WIDTH, HEIGHT, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glNamedFramebufferTexture(fboID, GL_DEPTH_ATTACHMENT, envTexture->texture_id, 0);
		//glNamedFramebufferDrawBuffers(fboID, 0, { GL_NONE });
		//glNamedFramebufferReadBuffer(fboID, GL_NONE);

		bind();
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		unbind();
	}
	else {

		envTexture->textureFormat = textureFormat; // GL_RGBA;
		envTexture->width = width; envTexture->height = height;

		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureStorage2D(envTexture->texture_id, 5, internalFormat, WIDTH, HEIGHT); //Safe GL_RGBA16
		glGenerateTextureMipmap(envTexture->texture_id); // random garbage color when using GL_RGBA16F

		glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT0, envTexture->texture_id, 0);
	}

	// Check if the framebuffer is complete before continuing
	auto status = glCheckNamedFramebufferStatus(fboID, GL_FRAMEBUFFER);
	//GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "CubeFBO not complete !" << std::endl;
	}
}