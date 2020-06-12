#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &id);
	indices_buffer = nullptr;
	num_triangles = 0;
}

VAO::~VAO()
{
	clear();
}

void VAO::clear()
{
	attribute_buffers.clear();
}

void VAO::storeIndices(std::vector<glm::ivec3> & indices)
{
	indices_buffer = std::make_unique<VBO>(GL_ELEMENT_ARRAY_BUFFER);

	bind();
		indices_buffer->bind();
		indices_buffer->setData(&indices[0], indices.size() * sizeof(glm::ivec3));
		indices_buffer->bind();
	unbind();

	num_triangles = indices.size();
}

void VAO::storeAttribute(Attribute c, int num_dimensions, GLvoid* data, size_t size_in_bytes, GLuint shader_location)
{
	if (attribute_buffers.find(c) != attribute_buffers.end())
		attribute_buffers[c] = NULL;
	
	attribute_buffers[c] = std::make_unique<VBO>(GL_ARRAY_BUFFER);

	bind();
		attribute_buffers[c]->bind();
		attribute_buffers[c]->setData(data, size_in_bytes);
			glEnableVertexAttribArray(shader_location);
			glVertexAttribPointer(shader_location, num_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
		attribute_buffers[c]->unbind();
	unbind();
}

void VAO::storePositions(std::vector<glm::vec3>& data, GLuint shader_location)
{
	storeAttribute(Attribute::POSITION, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void VAO::storeNormals(std::vector<glm::vec3>& data, GLuint shader_location)
{
	storeAttribute(Attribute::NORMAL, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void VAO::storeTexcoords(std::vector<glm::vec2>& data, GLuint shader_location)
{
	storeAttribute(Attribute::TEXTURE_COORDINATE, 2, &data[0], data.size() * sizeof(glm::vec2), shader_location);
}

void VAO::storeTangents(std::vector<glm::vec3>& data, GLuint shader_location)
{
	storeAttribute(Attribute::TANGENT, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void VAO::draw()
{
	bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(num_triangles * 3), GL_UNSIGNED_INT, 0);
	unbind();
}

void VAO::draw(size_t start, size_t end )
{
	bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(end - start) * 3, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * start * 3));
	unbind();
}

void VAO::bind()
{
	glBindVertexArray(id);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}
