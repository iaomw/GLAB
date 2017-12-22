#pragma once
#include <GL/glew.h>

class myVBO
{
public:
	myVBO(GLenum);
	~myVBO();

	void bind() const;
	void unbind() const;
	void setData(GLvoid *data, size_t size);

	GLuint buffer_id;
	GLenum buffer_type;
};

