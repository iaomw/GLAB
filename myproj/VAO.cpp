#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &id);

	//indices_buffer = std::make_unique<VBO>(GL_ELEMENT_ARRAY_BUFFER);
	indirect_buffer = std::make_unique<VBO>(GL_DRAW_INDIRECT_BUFFER, GL_DYNAMIC_DRAW);

	model_matrix_buffer = std::make_unique<VBO>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	normal_matrix_buffer = std::make_unique<VBO>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
}

VAO::~VAO()
{
	clear();
}

void VAO::clear()
{
	attribute_buffers.clear();
}

void VAO::storeIndices(std::vector<glm::ivec3>& indices)
{
	indices_buffer = std::make_unique<VBO>(GL_ELEMENT_ARRAY_BUFFER);

	bind();
		indices_buffer->bind();
		indices_buffer->setData(indices.data(), indices.size() * sizeof(glm::ivec3));
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
	storeAttribute(Attribute::POSITION, 3, data.data(), data.size() * sizeof(glm::vec3), shader_location);

	num_vertices = data.size();
}

void VAO::storeNormals(std::vector<glm::vec3>& data, GLuint shader_location)
{
	storeAttribute(Attribute::NORMAL, 3, data.data(), data.size() * sizeof(glm::vec3), shader_location);
}

void VAO::storeTexcoords(std::vector<glm::vec2>& data, GLuint shader_location)
{
	storeAttribute(Attribute::TEXTURE_COORDINATE, 2, data.data(), data.size() * sizeof(glm::vec2), shader_location);
}

void VAO::storeTangents(std::vector<glm::vec3>& data, GLuint shader_location)
{
	storeAttribute(Attribute::TANGENT, 3, data.data(), data.size() * sizeof(glm::vec3), shader_location);
}

void VAO::draw()
{
	bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(num_triangles * 3), GL_UNSIGNED_INT, 0);
	unbind();
}

void VAO::draw(size_t start, size_t end)
{
	bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(end - start) * 3, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * start * 3));
	unbind();
}

struct DrawCommand
{
	GLuint vertexCount;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
};

void VAO::draw(size_t start, size_t end, std::vector<glm::mat4>& model_matrix_list, std::vector<glm::mat4>& normal_matrix_list)
{
	bind();

	{
		model_matrix_buffer->bind();
		model_matrix_buffer->setData(model_matrix_list.data(), sizeof(glm::mat4) * model_matrix_list.size());

		glEnableVertexAttribArray(4 + 0);
		glEnableVertexAttribArray(4 + 1);
		glEnableVertexAttribArray(4 + 2);
		glEnableVertexAttribArray(4 + 3);

		glVertexAttribPointer(4 + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 0));
		glVertexAttribPointer(4 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 4));
		glVertexAttribPointer(4 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 8));
		glVertexAttribPointer(4 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 12));

		glVertexAttribDivisor(4 + 0, 1);
		glVertexAttribDivisor(4 + 1, 1);
		glVertexAttribDivisor(4 + 2, 1);
		glVertexAttribDivisor(4 + 3, 1);

		model_matrix_buffer->unbind(); 

		normal_matrix_buffer->bind();
		normal_matrix_buffer->setData(normal_matrix_list.data(), sizeof(glm::mat4) * normal_matrix_list.size());

		glEnableVertexAttribArray(8 + 0);
		glEnableVertexAttribArray(8 + 1);
		glEnableVertexAttribArray(8 + 2);
		glEnableVertexAttribArray(8 + 3);

		glVertexAttribPointer(8 + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 0));
		glVertexAttribPointer(8 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 4));
		glVertexAttribPointer(8 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 8));
		glVertexAttribPointer(8 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(GL_FLOAT) * 12));

		glVertexAttribDivisor(8 + 0, 1);
		glVertexAttribDivisor(8 + 1, 1);
		glVertexAttribDivisor(8 + 2, 1);
		glVertexAttribDivisor(8 + 3, 1);

		normal_matrix_buffer->unbind();
	}
	
	indirect_buffer->bind();

	{	std::vector<DrawCommand> drawList(model_matrix_list.size());
		//drawList.reserve(model_matrix_list.size());

		for (int i = 0; i < model_matrix_list.size(); ++i) {

			DrawCommand& command = drawList[i];

			command.vertexCount = num_triangles * 3; // indice count?
			command.instanceCount = 1;
			command.firstIndex = 0;

			command.baseVertex = 0;
			command.baseInstance = i;

			//command.baseVertex = num_vertices * i;
			//command.baseVertex = num_triangles * i; // point count?
			//command.baseVertex = num_vertices * i * 3;
			//command.baseVertex = num_triangles * i * 3; // point count?
		}

		indirect_buffer->setData(drawList.data(), sizeof(DrawCommand) * drawList.size());

		glBindBuffer(GL_ARRAY_BUFFER, indirect_buffer->getID());
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(DrawCommand), (void*)(offsetof(DrawCommand, baseInstance)));
		glVertexAttribDivisor(3, 1); //only once per instance
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glMultiDrawElementsIndirect(GL_TRIANGLES, //type
		GL_UNSIGNED_INT, //indices represented as unsigned ints
		0, //start with the first draw command
		model_matrix_list.size(), //draw 100 objects
		0); //no stride, the draw commands are tightly packed

	indirect_buffer->unbind();
	unbind();
}

inline void VAO::bind()
{
	glBindVertexArray(id);
}

inline void VAO::unbind()
{
	glBindVertexArray(0);
}
