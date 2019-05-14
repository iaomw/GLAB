#pragma once
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

	void initFBO(int WIDTH, int HEIGHT);
	void bind() const;
	void unbind() const;
	void clear();

	int getWidth();
	int getHeight();

private:
		int width;
		int height;
};

