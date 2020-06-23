#include "VBO.h"

VBO::VBO(GLenum type, GLenum usage)
{
	previous_size = 0;
	this->usage = usage;
	this->buffer_type = type;
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

	if (size_in_bytes != previous_size) {
		glNamedBufferData(buffer_id, size_in_bytes, data, this->usage);
		previous_size = size_in_bytes;
	}
	else {
		glNamedBufferSubData(buffer_id, 0, size_in_bytes, data);
	}
}

void VBO::setData(GLvoid* data, size_t offset, size_t size_in_bytes)
{
	glNamedBufferSubData(buffer_id, offset, size_in_bytes, data);
}