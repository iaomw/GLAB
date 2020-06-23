#pragma once
#include <GL/glew.h>

class VBO
{
public:

	VBO(GLenum type, GLenum usage = GL_STATIC_DRAW);
	~VBO();

	GLuint getID() { return buffer_id; }
	size_t byteSize() { return  total_byte_size; }

	void bind() const;
	void unbind() const;
	bool setData(GLvoid *data, size_t byte_size);
	bool setData(GLvoid* data, size_t offset, size_t byte_size);

protected:
	GLuint buffer_id;
	GLenum buffer_type;

	GLenum usage;
	size_t total_byte_size;
};
