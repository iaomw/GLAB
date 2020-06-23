#include "VBO.h"

VBO::VBO(GLenum type, GLenum usage)
{
	total_byte_size = 0;
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

bool VBO::setData(GLvoid * data, size_t byte_size)
{
	//glBufferData(buffer_type, size_in_bytes, data, GL_STATIC_DRAW);

	if (byte_size != total_byte_size) {
		glNamedBufferData(buffer_id, byte_size, data, this->usage);
		total_byte_size = byte_size;
	}
	else {
		glNamedBufferSubData(buffer_id, 0, byte_size, data);
	}
	return true;
}

bool VBO::setData(GLvoid* data, size_t offset, size_t byte_size)
{
	if ((offset + byte_size) > total_byte_size) { return false; }
		
	glNamedBufferSubData(buffer_id, offset, byte_size, data);
	return true;
}