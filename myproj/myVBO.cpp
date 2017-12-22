#include "myVBO.h"

myVBO::myVBO(GLenum type)
{
	buffer_type = type;
	glGenBuffers(1, &buffer_id);
}

myVBO::~myVBO()
{
	glDeleteBuffers(1, &buffer_id);
}

void myVBO::bind() const
{
	glBindBuffer(buffer_type, buffer_id);
}

void myVBO::unbind() const
{
	glBindBuffer(buffer_type, 0);
}

void myVBO::setData(GLvoid * data, size_t size_in_bytes)
{
	glBufferData(buffer_type, size_in_bytes, data, GL_STATIC_DRAW);
}
