#pragma once
#include <GL/glew.h>
#include "myTexture.h"

class myFBO
{
public:
	myTexture *colortexture;
	myTexture *depthtexture;
	GLuint fboid;

	myFBO();
	~myFBO();

	void initFBO(int width, int height);
	void bind() const;
	void unbind() const;
	void clear();
};

