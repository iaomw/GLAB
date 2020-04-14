#pragma once
#include <GL/glew.h>
#include "myTexture.h"

class CubeFBO
{
public:

	GLuint fboID;
	myTexture* envTexture;
	CubeFBO();
	~CubeFBO();

	void initFBO(const int& WIDTH, const int& HEIGHT, const GLenum format);
	void bind() const;
	void unbind() const;
	void clear();

	int getWidth();
	int getHeight();

private:
		int width;
		int height;
};

