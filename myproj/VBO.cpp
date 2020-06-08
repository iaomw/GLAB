#include "VBO.h"

VBO::VBO(GLenum type)
{
	buffer_type = type;
	//glGenBuffers(1, &buffer_id);
	glCreateBuffers(1, &buffer_id);
}

VBO::~VBO()
{
	glDeleteBuffers(1, &buffer_id);
}

void VBO::bind() const
{
	glBindBuffer(buffer_type, buffer_id);
}

void VBO::unbind() const
{
	glBindBuffer(buffer_type, 0);
}

void VBO::setData(GLvoid * data, size_t size_in_bytes)
{
	//glBufferData(buffer_type, size_in_bytes, data, GL_STATIC_DRAW);
	glNamedBufferData(buffer_id, size_in_bytes, data, GL_STATIC_DRAW);
}
