#pragma once

#include "VBO.h"

#include <map>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

class VAO
{
private:
	enum struct Attribute { POSITION, NORMAL, TANGENT, TEXTURE_COORDINATE };
	
public:
	GLuint id;

	std::map<Attribute, std::unique_ptr<VBO>> attribute_buffers;
	std::unique_ptr<VBO> indices_buffer;
	size_t num_triangles;

	VAO();
	~VAO();

	void clear();
	
	void storeAttribute(Attribute c, int num_dimensions, GLvoid *data, size_t size_in_bytes, GLuint shader_location);

	void storeIndices(std::vector<glm::ivec3>&);
	void storePositions(std::vector<glm::vec3>&, GLuint shader_location);
	void storeNormals(std::vector<glm::vec3>&, GLuint shader_location);
	void storeTexcoords(std::vector<glm::vec2>&, GLuint shader_location);
	void storeTangents(std::vector<glm::vec3>&, GLuint shader_location);
	
	void draw();
	void draw(size_t start, size_t end);

	void bind();
	void unbind();
};

