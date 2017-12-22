#pragma once
#include <GL/glew.h>
#include "myTexture.h"

class gFBO
{
public:

	GLuint gBuffer, zBuffer;

	myTexture* gPosition;
	myTexture* gNormal;
	myTexture* gAlbedo;
	myTexture* gEnv;

	gFBO();
	~gFBO();

	void initFBO(int WIDTH, int HEIGHT);
	void bind() const;
	void unbind() const;
	void clear();
};

