#include "gFBO.h"
#include <iostream>



gFBO::gFBO()
{
	gBuffer = 0;
	zBuffer = 0;

	gPosition = nullptr;
	gNormal = nullptr;
	gAlbedo = nullptr;
	gEnv = nullptr;
}


gFBO::~gFBO()
{
	
	if (gBuffer != 0) glDeleteFramebuffers(1, &gBuffer);
	if (zBuffer != 0) glDeleteRenderbuffers(1, &zBuffer);

	if (gPosition != nullptr) delete gPosition;
	if (gNormal != nullptr) delete gNormal;
	if (gAlbedo != nullptr) delete gAlbedo;
	if (gEnv != nullptr) delete gEnv;
}

void gFBO::initFBO(int WIDTH, int HEIGHT)
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// Position
	if (gPosition) delete gPosition;
	gPosition = new myTexture();

	glGenTextures(1, &gPosition->texture_id);
	glBindTexture(GL_TEXTURE_2D, gPosition->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition->texture_id, 0);

	// Albedo + Roughness
	if (gAlbedo) delete gAlbedo;
	gAlbedo = new myTexture();

	glGenTextures(1, &gAlbedo->texture_id);
	glBindTexture(GL_TEXTURE_2D, gAlbedo->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo->texture_id, 0);

	// Normals + Metalness
	if (gNormal) delete gNormal;
	gNormal = new myTexture();

	glGenTextures(1, &gNormal->texture_id);
	glBindTexture(GL_TEXTURE_2D, gNormal->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal->texture_id, 0);

	// Env Map result
	if (gEnv) delete gEnv;
	gEnv = new myTexture();

	glGenTextures(1, &gEnv->texture_id);
	glBindTexture(GL_TEXTURE_2D, gEnv->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEnv->texture_id, 0);

	// Define the COLOR_ATTACHMENTS for the G-Buffer
	GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// Z-Buffer
	glGenRenderbuffers(1, &zBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, zBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zBuffer);

	// Check if the framebuffer is complete before continuing
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete !" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gFBO::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
}

void gFBO::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gFBO::clear()
{
	bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	unbind();
}
