#include "FBO.h"
#include <iostream>

FBO::FBO(bool extra, bool mipmap)
{
	needExtra = extra;
	needMipmap = mipmap;

	fboID = 0;
	width = 0; height = 0;
	colorTexture = nullptr;
	extraTexture = nullptr;
	depthTexture = nullptr;
}

FBO::~FBO()
{
	reset();
}

void FBO::reset() {

	if (fboID != 0) glDeleteFramebuffers(1, &fboID); fboID = 0;
}

void FBO::initFBO(int WIDTH, int HEIGHT) 
{
	width = WIDTH; height = HEIGHT;

	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	glCreateFramebuffers(1, &fboID);

	colorTexture = std::make_shared<Texture>();

	//GL_NEAREST_MIPMAP_NEAREST 
	//GL_LINEAR_MIPMAP_NEAREST 
	//GL_NEAREST_MIPMAP_LINEAR 
	//GL_LINEAR_MIPMAP_LINEAR 

	auto mini_filter = GL_LINEAR;
	auto numLevels = 1;
	
	if (needMipmap) {
		mini_filter = GL_LINEAR_MIPMAP_LINEAR;
		numLevels = 1 + floor(log2((double)std::max(width, height)));
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &colorTexture->texture_id);
	glTextureStorage2D(colorTexture->texture_id, numLevels, GL_RGBA16F, width, height);

	glTextureParameteri(colorTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(colorTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(colorTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(colorTexture->texture_id, GL_TEXTURE_MIN_FILTER, mini_filter);

	glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT0, colorTexture->texture_id, 0);

	depthTexture = std::make_shared<Texture>();

	glCreateTextures(GL_TEXTURE_2D, 1, &depthTexture->texture_id);
	glTextureStorage2D(depthTexture->texture_id, 1, GL_DEPTH_COMPONENT32, width, height);

	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glNamedFramebufferTexture(fboID, GL_DEPTH_ATTACHMENT, depthTexture->texture_id, 0);

	if (!needExtra)
	{
		unsigned int attachment[1] = { GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers(fboID, 1, attachment);
	}
	else 
	{
		extraTexture = std::make_shared<Texture>();

		glCreateTextures(GL_TEXTURE_2D, 1, &extraTexture->texture_id);
		glTextureStorage2D(extraTexture->texture_id, 1, GL_RGBA16F, width, height);

		glTextureParameteri(extraTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(extraTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(extraTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(extraTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT1, extraTexture->texture_id, 0);

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glNamedFramebufferDrawBuffers(fboID, 2, attachments);
	}

	// finally check if framebuffer is complete
	if (glCheckNamedFramebufferStatus(fboID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("incomplete framebuffer");
	}
}