#pragma once

#include <GL/glew.h>

class VBO
{
public:
	VBO(GLenum type, GLenum usage = GL_STATIC_DRAW);
	~VBO();

	void bind() const;
	void unbind() const;
	void setData(GLvoid *data, size_t size);

	GLuint buffer_id;
	GLenum buffer_type;

private:
	GLenum usage;
	size_t previous_size = 0;
};
