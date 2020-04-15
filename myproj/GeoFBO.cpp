#include "GeoFBO.h"
#include <iostream>

GeoFBO::GeoFBO() //: FBO()
{
	gPosition = nullptr;
	gNormal = nullptr;
	gAlbedo = nullptr;
}


GeoFBO::~GeoFBO()
{
	if (gPosition != nullptr) delete gPosition;
	if (gNormal != nullptr) delete gNormal;
	if (gAlbedo != nullptr) delete gAlbedo;
}

void GeoFBO::initFBO(int WIDTH, int HEIGHT)
{
	width = WIDTH; height = HEIGHT;

	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

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
	glBindTexture(GL_TEXTURE_2D, 0);

	// Z-Buffer
	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);

	// Check if the framebuffer is complete before continuing
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GeoFBO not complete !" << std::endl;
	}
	unbind();
}
