#include "GeoFBO.h"
#include <iostream>



GeoFBO::GeoFBO()
{
	gBuffer = 0;
	zBuffer = 0;

	gPosition = nullptr;
	gNormal = nullptr;
	gAlbedo = nullptr;
}


GeoFBO::~GeoFBO()
{
	
	if (gBuffer != 0) glDeleteFramebuffers(1, &gBuffer);
	if (zBuffer != 0) glDeleteRenderbuffers(1, &zBuffer);

	if (gPosition != nullptr) delete gPosition;
	if (gNormal != nullptr) delete gNormal;
	if (gAlbedo != nullptr) delete gAlbedo;
}

void GeoFBO::initFBO(int WIDTH, int HEIGHT)
{
	if (gBuffer != 0) glDeleteFramebuffers(1, &gBuffer);
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// Position
	if (gPosition) delete gPosition;
	gPosition = new myTexture();

	glGenTextures(1, &gPosition->texture_id);
	glBindTexture(GL_TEXTURE_2D, gPosition->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition->texture_id, 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Albedo + Roughness
	if (gAlbedo) delete gAlbedo;
	gAlbedo = new myTexture();

	glGenTextures(1, &gAlbedo->texture_id);
	glBindTexture(GL_TEXTURE_2D, gAlbedo->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo->texture_id, 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Normals + Metalness
	if (gNormal) delete gNormal;
	gNormal = new myTexture();

	glGenTextures(1, &gNormal->texture_id);
	glBindTexture(GL_TEXTURE_2D, gNormal->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal->texture_id, 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Define the COLOR_ATTACHMENTS for the G-Buffer
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// Z-Buffer
	glGenRenderbuffers(1, &zBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, zBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zBuffer);

	// Check if the framebuffer is complete before continuing
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GeoFBO not complete !" << std::endl;
	}

	unbind();
}

/*
void GeoFBO::initFBO(int WIDTH, int HEIGHT)
{
	if (gBuffer != 0) glDeleteFramebuffers(1, &gBuffer);
	glCreateFramebuffers(1, &gBuffer);

	// Position
	if (gPosition) delete gPosition;
	gPosition = new myTexture();
	glCreateTextures(GL_TEXTURE_2D, 1, &(gPosition->texture_id));
	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(gPosition->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(gPosition->texture_id, 1, GL_RGBA16F, WIDTH, HEIGHT);
	//glGenerateTextureMipmap(gPosition->texture_id);
	glNamedFramebufferTexture(gBuffer, GL_COLOR_ATTACHMENT0, gPosition->texture_id, 0);

	//glGenTextures(1, &tid);
	//glTextureImage2DEXT(gPosition->texture_id, GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	//glGenFramebuffers(1, &fbo);
	//glNamedFramebufferTexture2DEXT(gBuffer, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition->texture_id, 0);

	// Albedo + Roughness
	if (gAlbedo) delete gAlbedo;
	gAlbedo = new myTexture();
	glCreateTextures(GL_TEXTURE_2D, 1, &(gAlbedo->texture_id));
	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(gAlbedo->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(gAlbedo->texture_id, 1, GL_RGBA8, WIDTH, HEIGHT);
	//glGenerateTextureMipmap(gAlbedo->texture_id);
	glNamedFramebufferTexture(gBuffer, GL_COLOR_ATTACHMENT1, gAlbedo->texture_id, 0);

	//glGenTextures(1, &tid);
	//glTextureImage2DEXT(gAlbedo->texture_id, GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	//glGenFramebuffers(1, &fbo);
	//glNamedFramebufferTexture2DEXT(gBuffer, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo->texture_id, 0);

	// Normals + Metalness
	if (gNormal) delete gNormal;
	gNormal = new myTexture();
	glCreateTextures(GL_TEXTURE_2D, 1, &(gNormal->texture_id));
	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(gNormal->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(gNormal->texture_id, 1, GL_RGBA8, WIDTH, HEIGHT);
	//glGenerateTextureMipmap(gNormal->texture_id);
	glNamedFramebufferTexture(gBuffer, GL_COLOR_ATTACHMENT2, gNormal->texture_id, 0);

	// Z-Buffer
	//if (zBuffer != 0) glDeleteRenderbuffers(1, &zBuffer);
	//glCreateRenderbuffers(1, &zBuffer);
	//glNamedRenderbufferStorage(zBuffer, GL_RGBA16F, WIDTH, HEIGHT);
	//glNamedFramebufferRenderbuffer(gBuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zBuffer);

	// Check if the framebuffer is complete before continuing
	if (glCheckNamedFramebufferStatus(gBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete !" << std::endl;
	}
}
*/

void GeoFBO::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, zBuffer);
}

void GeoFBO::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void GeoFBO::clear()
{
	bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	unbind();
}
