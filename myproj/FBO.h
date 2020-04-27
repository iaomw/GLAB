#pragma once
#include <functional>
#include <GL/glew.h>

#include "myObject.h"
#include "myShader.h"
#include "myTexture.h"

class RenderTarget {

public:
	myTexture* colorTexture;
	myTexture* extraTexture;
	myTexture* depthTexture;

	GLuint fboID;
	int width, height;

	virtual ~RenderTarget() {}

	void render(myShader* shader, myObject* object, glm::mat4 view_matrix, 
		std::function<void()>* before=nullptr, std::function<void()>* after=nullptr) {

		bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (before != nullptr) {
			(*before)();
		}
		object->displayObjects(shader, view_matrix);
		shader->stop();
		if (after != nullptr) {
			(*after)();
		}
		unbind();
	}

	virtual inline void bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		//glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	}

	virtual inline void unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	virtual void clear() const
	{
		bind();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		unbind();
	}

	int getWidth() { return width;}
	int getHeight() { return height;}
};

class FBO: public RenderTarget
{
public:

	FBO(bool extra = false, bool mipmap = false);
	
	virtual ~FBO();
	
	void initFBO(int width, int height);
	virtual void reset();

private:
	bool needExtra;
	bool needMipmap;
};

