#pragma once
#include <GL/glew.h>

class VBO
{
public:

	VBO(GLenum type, GLenum usage = GL_STATIC_DRAW);
	~VBO();

	GLuint getID() { return buffer_id; }

	void bind() const;
	void unbind() const;
	void setData(GLvoid *data, size_t size);
	void setData(GLvoid* data, size_t offset, size_t size_in_bytes);

protected:
	GLuint buffer_id;
	GLenum buffer_type;

	GLenum usage;
	size_t previous_size;
};
