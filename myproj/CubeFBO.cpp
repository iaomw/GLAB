#include "CubeFBO.h"
#include <iostream>

CubeFBO::CubeFBO()
{
	envTexture = nullptr;
}

CubeFBO::~CubeFBO()
{

	if (fboID != 0) glDeleteFramebuffers(1, &fboID);

	if (envTexture != nullptr) delete envTexture;
}

void CubeFBO::initFBO(const int& WIDTH, const int& HEIGHT, const GLenum format)
{
	if (fboID != 0) glDeleteFramebuffers(1, &fboID);

	width = WIDTH; height = HEIGHT;

	glCreateFramebuffers(1, &fboID);

	if (envTexture) delete envTexture;
	envTexture = new myTexture(GL_TEXTURE_CUBE_MAP);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &(envTexture->texture_id));

	envTexture->texFormat = GL_RGBA;
	envTexture->width = width; envTexture->height = height;

	glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(envTexture->texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(envTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureStorage2D(envTexture->texture_id, 5, format, WIDTH, HEIGHT); //Safe GL_RGBA16
	glGenerateTextureMipmap(envTexture->texture_id); // random garbage color when using GL_RGBA16F

	/*for (size_t face = 0; face < 6; ++face)
	{
		glTextureSubImage3D(fboID, 0, 0, 0, face, WIDTH, HEIGHT, 1, GL_RGBA16F, GL_FLOAT, nullptr);
	}*/

	glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT0, envTexture->texture_id, 0);

	// Check if the framebuffer is complete before continuing
	auto status = glCheckNamedFramebufferStatus(fboID, GL_FRAMEBUFFER);
	//GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "CubeFBO not complete !" << std::endl;
	}
}

void CubeFBO::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void CubeFBO::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeFBO::clear()
{
	bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	unbind();
}

int CubeFBO::getWidth() { return width; }
int CubeFBO::getHeight() { return height; }