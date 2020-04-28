#pragma once

#include <Functional>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <map>
#include <vector>
#include <string>

class myShader
{
public:
	GLuint vertex_shader = 0;
	GLuint geometry_shader = 0;
	GLuint fragment_shader = 0;

	GLuint shaderprogram;

	std::map<std::string, GLint> text_to_id;

	myShader(const std::string& file_vertexshader, const std::string& file_fragmentshader);
	myShader(const std::string& file_vertexshader, const std::string& file_geometryshader, const std::string& file_fragmentshader);

	~myShader();

	void clear();
	void start() const;
	void stop() const;

	GLint getUniformLocation(const std::string& name);

	void setUniform(const std::string& name, int);
	void setUniform(const std::string& name, float);
	void setUniform(const std::string& name, glm::mat3&);
	void setUniform(const std::string& name, glm::mat4&);
	void setUniform(const std::string& name, glm::vec2);
	void setUniform(const std::string& name, glm::vec3);
	void setUniform(const std::string& name, glm::vec4);
	void setUniform(const std::string& name, std::vector<glm::vec3>& input_array);
	void setUniform(const std::string& name, std::vector<glm::fvec4>& input_array);
	

private:
	GLuint _initShader(GLenum type, const std::string& filename);
	bool _initProgram();
	void _programErrors(const GLint program);
	void _shaderErrors(const GLint shader);
};


