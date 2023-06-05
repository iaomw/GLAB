#include "GeoFBO.h"
#include <iostream>

GeoFBO::GeoFBO(): FBO()
{
	fboID = 0;
	width = 0; height = 0;

	texAlbedo = nullptr;
	texNormal = nullptr;
}

GeoFBO::~GeoFBO()
{
	reset();
}

void GeoFBO::reset() {
	if (fboID != 0) {
		glDeleteFramebuffers(1, &fboID); 
		fboID = 0;
	}

	texAlbedo.reset(); 
	texNormal.reset();
}

void GeoFBO::initFBO(int WIDTH, int HEIGHT)
{
	if (WIDTH == this->width && HEIGHT == this->height) {return;}

	this->width = WIDTH; this->height = HEIGHT;

	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	glCreateFramebuffers(1, &fboID);

	// Albedo + Roughness
	texAlbedo = std::make_unique<Texture>(GL_TEXTURE_2D);

	glCreateTextures(GL_TEXTURE_2D, 1, &texAlbedo->texture_id);
	glTextureStorage2D(texAlbedo->texture_id, 1, GL_RGBA16F, WIDTH, HEIGHT);

	glTextureParameteri(texAlbedo->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texAlbedo->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texAlbedo->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texAlbedo->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT0, texAlbedo->texture_id, 0);
	texAlbedo->getHandle();

	// Normals + Metalness
	texNormal = std::make_unique<Texture>(GL_TEXTURE_2D);

	glCreateTextures(GL_TEXTURE_2D, 1, &texNormal->texture_id);
	glTextureStorage2D(texNormal->texture_id, 1, GL_RGBA16F, WIDTH, HEIGHT);

	glTextureParameteri(texNormal->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texNormal->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texNormal->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texNormal->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT1, texNormal->texture_id, 0);
	texNormal->getHandle();

	depthTexture = std::make_unique<Texture>();

	glCreateTextures(GL_TEXTURE_2D, 1, &depthTexture->texture_id);
	glTextureStorage2D(depthTexture->texture_id, 1, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);

	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glNamedFramebufferTexture(fboID, GL_DEPTH_STENCIL_ATTACHMENT, depthTexture->texture_id, 0);
	
	std::vector<GLenum> attachments { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glNamedFramebufferDrawBuffers(fboID, (GLsizei)attachments.size(), attachments.data());

	// finally check if framebuffer is complete
	if (glCheckNamedFramebufferStatus(fboID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("incomplete framebuffer");
	}
}
