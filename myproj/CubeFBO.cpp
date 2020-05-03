#include "CubeFBO.h"
#include <iostream>

CubeFBO::CubeFBO(GLenum textureFormat, GLenum internalFormat) : FBO()
{
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

	// Check if the framebuffer is complete before continuing
	auto status = glCheckNamedFramebufferStatus(fboID, GL_FRAMEBUFFER);
	//GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "CubeFBO not complete !" << std::endl;
	}
}