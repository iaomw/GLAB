#include "myVAO.h"
#include <iostream>
#include <glm/vec3.hpp>
#include "errors.h"


myVAO::myVAO()
{
	glGenVertexArrays(1, &id);
	indices_buffer = nullptr;
	num_triangles = 0;
}


myVAO::~myVAO()
{
	clear();
}

void myVAO::clear()
{
	if (indices_buffer) delete indices_buffer;

	for (std::map<Attribute, myVBO *>::iterator it = attribute_buffers.begin(); it != attribute_buffers.end(); ++it)
		delete it->second;

	attribute_buffers.clear();
}

void myVAO::storeIndices(std::vector<glm::ivec3> & indices)
{
	if (indices_buffer)
		delete indices_buffer;

	indices_buffer = new myVBO(GL_ELEMENT_ARRAY_BUFFER);

	bind();
	indices_buffer->bind();
	indices_buffer->setData(&indices[0], indices.size() * sizeof(glm::ivec3));
	unbind();

	num_triangles = indices.size();
}

void myVAO::storeAttribute(Attribute c, int num_dimensions, GLvoid* data, size_t size_in_bytes, GLuint shader_location)
{
	if (attribute_buffers.find(c) != attribute_buffers.end())
		delete attribute_buffers[c];
	
	attribute_buffers[c] = new myVBO(GL_ARRAY_BUFFER);

	bind();
	attribute_buffers[c]->bind();
	attribute_buffers[c]->setData(data, size_in_bytes);
	glVertexAttribPointer(shader_location, num_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader_location);
	unbind();
}


void myVAO::storePositions(std::vector<glm::vec3> data, GLuint shader_location)
{
	storeAttribute(POSITION, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void myVAO::storeNormals(std::vector<glm::vec3> data, GLuint shader_location)
{
	storeAttribute(NORMAL, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void myVAO::storeTexturecoordinates(std::vector<glm::vec2> data, GLuint shader_location)
{
	storeAttribute(TEXTURE_COORDINATE, 2, &data[0], data.size() * sizeof(glm::vec2), shader_location);
}

void myVAO::storeTangents(std::vector<glm::vec3> data, GLuint shader_location)
{
	storeAttribute(TANGENT, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void myVAO::draw()
{
	bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(num_triangles * 3), GL_UNSIGNED_INT, 0);
	unbind();
}

void myVAO::draw(size_t start, size_t end )
{
	bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(end - start) * 3, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * start * 3));
	unbind();
}

void myVAO::bind()
{
	glBindVertexArray(id);
}

void myVAO::unbind()
{
	glBindVertexArray(0);
}
