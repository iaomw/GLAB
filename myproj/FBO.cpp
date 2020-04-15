#include "FBO.h"
#include <iostream>

FBO::FBO(bool useExtra)
{
	needExtra = useExtra;

	fboID = 0; rboID = 0;
	width = 0; height = 0;
	colortexture = nullptr;
	extratexture = nullptr;
}


FBO::~FBO()
{
	if (colortexture != nullptr) delete colortexture;
	if (extratexture != nullptr) delete extratexture;
	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	if (rboID != 0) glDeleteFramebuffers(1, &rboID);
}

void FBO::initFBO(int width, int height) 
{
	this->width = width;
	this->height = height;

	if (fboID != 0) glDeleteFramebuffers(1, &fboID);
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	if (colortexture != nullptr) delete colortexture;
	colortexture = new myTexture();

	glGenTextures(1, &colortexture->texture_id);
	glBindTexture(GL_TEXTURE_2D, colortexture->texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colortexture->texture_id, 0);

	if (needExtra)
	{
		if (extratexture != nullptr) delete extratexture;
		extratexture = new myTexture();

		glGenTextures(1, &extratexture->texture_id);
		glBindTexture(GL_TEXTURE_2D, extratexture->texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, extratexture->texture_id, 0);

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
	}
	else {
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	if (rboID != 0) glDeleteRenderbuffers(1, &rboID);
	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "FBO not complete!" << std::endl;
	}
	unbind();
}