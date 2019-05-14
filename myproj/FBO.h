#pragma once
#include <GL/glew.h>
#include "myTexture.h"

class FBO
{
public:
	myTexture *colortexture;
	GLuint fboID, rboID;

	FBO();
	~FBO();

	void initFBO(int width, int height);
	void bind() const;
	void unbind() const;
	void clear();

	int getWidth();
	int getHeight();

private:
	int width, height;
};

