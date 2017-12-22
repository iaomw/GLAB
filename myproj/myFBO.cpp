#include "myFBO.h"
#include <iostream>


myFBO::myFBO()
{
	colortexture = nullptr;
	depthtexture = nullptr;
	fboid = 0;
}


myFBO::~myFBO()
{
	if (colortexture != nullptr) delete colortexture;
	if (depthtexture != nullptr) delete depthtexture;
	if (fboid != 0) glDeleteFramebuffers(1, &fboid);
}

void myFBO::initFBO(int width, int height)
{
	if (colortexture) delete colortexture;
	colortexture = new myTexture();

	glGenTextures(1, &colortexture->texture_id);
	glBindTexture(GL_TEXTURE_2D, colortexture->texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	if (depthtexture) delete depthtexture;
	depthtexture = new myTexture();

	// color + specular color buffer
	glGenTextures(1, &depthtexture->texture_id);
	glBindTexture(GL_TEXTURE_2D, depthtexture->texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	glGenFramebuffers(1, &fboid);
	glBindFramebuffer(GL_FRAMEBUFFER, fboid);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colortexture->texture_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, depthtexture->texture_id, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		//std::cout << "Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void myFBO::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboid);
}

void myFBO::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void myFBO::clear()
{
	bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	unbind();
}
