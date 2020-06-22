#include "CubeFBO.h"

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

	envTexture.reset();
}

void CubeFBO::initFBO(const int& WIDTH, const int& HEIGHT)
{
	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	glCreateFramebuffers(1, &fboID);
	width = WIDTH; height = HEIGHT;

	envTexture = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &(envTexture->texture_id));

	if (shadow) {

		if (!GL_ARB_bindless_texture) {
			std::cout << "GL_ARB_bindless_texture not available." << std::endl;
			exit(1);
		}

		envTexture->textureFormat = GL_DEPTH_COMPONENT;
		envTexture->width = width; envTexture->height = height;

		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_CUBE_MAP, envTexture->texture_id);

		for (GLenum face = 0; face < 6; ++face)
		{
			//glTextureSubImage3D(envTexture->texture_id, 0, 0, 0, face, WIDTH, HEIGHT, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

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

		glTextureStorage2D(envTexture->texture_id, mipmap_level, internalFormat, WIDTH, HEIGHT); 
		// When mipmap_level is smaller than 6, there will be random garbage color in mipmap;
		glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT0, envTexture->texture_id, 0);
		//glGenerateTextureMipmap(envTexture->texture_id);
	}

	envTexture->getHandle();

	// Check if the framebuffer is complete before continuing
	auto status = glCheckNamedFramebufferStatus(fboID, GL_FRAMEBUFFER);
	//GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "CubeFBO not complete !" << std::endl;
	}
}