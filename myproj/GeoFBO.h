#pragma once
#include <GL/glew.h>
#include "myTexture.h"

class GeoFBO
{
public:

	GLuint gBuffer, zBuffer;

	myTexture* gPosition;
	myTexture* gNormal;
	myTexture* gAlbedo;

	GeoFBO();
	~GeoFBO();

	void initFBO(int WIDTH, int HEIGHT);
	void bind() const;
	void unbind() const;
	void clear();
};

