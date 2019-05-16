#pragma once
#include <GL/glew.h>
#include "myTexture.h"

class FBO
{
public:
	myTexture *colortexture;
	myTexture *extratexture;
	GLuint fboID, rboID;

	FBO(bool useExtra = false);
	~FBO();

	void initFBO(int width, int height);
	void bind() const;
	void unbind() const;
	void clear();

	int getWidth();
	int getHeight();

private:
	bool needExtra;
	int width, height;
};

