#pragma once
#include <functional>
#include <GL/glew.h>

#include "myObject.h"
#include "myShader.h"
#include "myTexture.h"

class FBO
{
public:
	myTexture* colorTexture;
	myTexture* extraTexture;
	myTexture* depthTexture;

	GLuint fboID;
	int width, height;

	void render(myShader* shader, myObject* object, glm::mat4 view_matrix,
		std::function<void()>* before = nullptr, std::function<void()>* after = nullptr) {

		bind();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->start();
		if (before != nullptr) {
			(*before)();
		}
		object->displayObjects(shader, view_matrix);
		shader->stop();

		if (after != nullptr) {
			(*after)();
		}
		unbind();

		if (needMipmap) {
			glGenerateTextureMipmap(colorTexture->texture_id);
			if (needExtra) {
				glGenerateTextureMipmap(extraTexture->texture_id);
			}
		}
	}

	void multi_render(std::function<void()>* work = nullptr) {

		bind();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (work != nullptr) {
			(*work)();
		}

		unbind();

		if (needMipmap) {
			glGenerateTextureMipmap(colorTexture->texture_id);
			if (needExtra) {
				glGenerateTextureMipmap(extraTexture->texture_id);
			}
		}
	}

	virtual inline void bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	}

	virtual inline void unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void clear() const
	{
		bind();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		unbind();
	}

	int getWidth() { return width; }
	int getHeight() { return height; }

	FBO(bool extra = false, bool mipmap = false);
	virtual ~FBO();
	
	void initFBO(int width, int height);
	virtual void reset();

private:
	bool needExtra;
	bool needMipmap;
};

