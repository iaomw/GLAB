#include "GeoFBO.h"
#include <iostream>

GeoFBO::GeoFBO(): FBO()
{
	fboID = 0;
	width = 0; height = 0;

	gPosition = nullptr;
	gAlbedo = nullptr;
	gNormal = nullptr;
}


GeoFBO::~GeoFBO()
{
	reset();
}

void GeoFBO::reset() {
	if (fboID != 0) glDeleteFramebuffers(1, &fboID); fboID = 0;

	delete gPosition; gPosition = nullptr;
	delete gAlbedo; gAlbedo = nullptr;
	delete gNormal; gNormal = nullptr;
}

void GeoFBO::initFBO(int WIDTH, int HEIGHT)
{
	width = WIDTH; height = HEIGHT;

	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	glCreateFramebuffers(1, &fboID);

	// Position + AO
	if (gPosition) delete gPosition;
	gPosition = new myTexture();

	glCreateTextures(GL_TEXTURE_2D, 1, &gPosition->texture_id);
	glTextureStorage2D(gPosition->texture_id, 1, GL_RGBA16F, WIDTH, HEIGHT);

	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT0, gPosition->texture_id, 0);
	//glGenerateTextureMipmap(gPosition->texture_id);

	// Albedo + Roughness
	if (gAlbedo) delete gAlbedo;
	gAlbedo = new myTexture();

	glCreateTextures(GL_TEXTURE_2D, 1, &gAlbedo->texture_id);
	glTextureStorage2D(gAlbedo->texture_id, 1, GL_RGBA16F, WIDTH, HEIGHT);

	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT1, gAlbedo->texture_id, 0);
	//glGenerateTextureMipmap(gAlbedo->texture_id);

	// Normals + Metalness
	if (gNormal) delete gNormal;
	gNormal = new myTexture();

	glCreateTextures(GL_TEXTURE_2D, 1, &gNormal->texture_id);
	glTextureStorage2D(gNormal->texture_id, 1, GL_RGBA16F, WIDTH, HEIGHT);

	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glNamedFramebufferTexture(fboID, GL_COLOR_ATTACHMENT2, gNormal->texture_id, 0);
	//glGenerateTextureMipmap(gNormal->texture_id);

	delete depthTexture;
	depthTexture = new myTexture();

	glCreateTextures(GL_TEXTURE_2D, 1, &depthTexture->texture_id);
	glTextureStorage2D(depthTexture->texture_id, 1, GL_DEPTH_COMPONENT32, width, height);

	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(depthTexture->texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glNamedFramebufferTexture(fboID, GL_DEPTH_ATTACHMENT, depthTexture->texture_id, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glNamedFramebufferDrawBuffers(fboID, 3, attachments);

	// finally check if framebuffer is complete
	if (glCheckNamedFramebufferStatus(fboID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("incomplete framebuffer");
	}
}
