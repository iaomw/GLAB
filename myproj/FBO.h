#pragma once
#include <functional>

#include "Shader.h"
#include "Texture.h"
#include "MeshPack.h"

class FBO
{
public:

	std::unique_ptr<Texture> colorTexture;
	std::unique_ptr<Texture> extraTexture;
	std::unique_ptr<Texture> depthTexture;

	void monoDraw(std::unique_ptr<Shader> const& shader, 
		std::unique_ptr<MeshPack> const &object, glm::mat4 view_matrix,
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

	void multi_render(std::function<void()> work) {

		bind();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		work();

		unbind();

		if (needMipmap) {
			glGenerateTextureMipmap(colorTexture->texture_id);
			if (needExtra) {
				glGenerateTextureMipmap(extraTexture->texture_id);
			}
		}
	}

public:

	FBO(bool extra = false, bool mipmap = false);
	virtual ~FBO();

	void initFBO(int width, int height);
	virtual void reset();

	int getWidth() { return width; }
	int getHeight() { return height; }

protected:

	GLuint fboID;
	bool needExtra;
	bool needMipmap;

	uint32_t width, height;

public:
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
};

