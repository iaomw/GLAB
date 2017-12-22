#pragma once
#include <GL/glew.h>
#include <map>
#include <vector>
#include "myVBO.h"
#include <glm/glm.hpp>

class myVAO
{
private:
	enum Attribute { POSITION, NORMAL, TANGENT, TEXTURE_COORDINATE };
	
public:
	GLuint id;

	std::map<Attribute, myVBO *> attribute_buffers;
	myVBO *indices_buffer;
	size_t num_triangles;

	myVAO();
	~myVAO();

	void clear();
	
	void storeAttribute(Attribute c, int num_dimensions, GLvoid *data, size_t size_in_bytes, GLuint shader_location);

	void storeIndices(std::vector<glm::ivec3> &);
	void storePositions(std::vector<glm::vec3>, GLuint shader_location);
	void storeNormals(std::vector<glm::vec3>, GLuint shader_location);
	void storeTexturecoordinates(std::vector<glm::vec2>, GLuint shader_location);
	void storeTangents(std::vector<glm::vec3>, GLuint shader_location);
	
	void draw();
	void draw(size_t start, size_t end);

	void bind();
	void unbind();
};

