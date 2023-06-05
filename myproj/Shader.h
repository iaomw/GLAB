#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <Functional>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>  

#include "magic_enum.hpp"

class Shader
{
public:

	Shader(const std::string& key);
	Shader(const std::string& file_vertexshader, const std::string& file_fragmentshader);
	Shader(const std::string& file_vertexshader, const std::string& file_geometryshader, const std::string& file_fragmentshader);

	~Shader();

	void clear();
	void start() const;
	void stop() const;

	GLint getUniformLocation(const std::string& name);

	void setUniform(const std::string& name, int);
	void setUniform(const std::string& name, float);
	void setUniform(const std::string& name, glm::mat3&);
	void setUniform(const std::string& name, glm::mat4&);
	void setUniform(const std::string& name, glm::vec2);
	void setUniform(const std::string& name, glm::vec3&);
	void setUniform(const std::string& name, glm::vec4&);
	void setUniform(const std::string& name, std::vector<glm::vec3>& input_array);
	void setUniform(const std::string& name, std::vector<glm::vec4>& input_array);

	void setTex(const std::string& name, GLuint texture_id, GLuint texture_offset);
	
	GLuint shaderprogram;

private:
	
	GLuint vertex_shader = 0;
	GLuint geometry_shader = 0;
	GLuint fragment_shader = 0;

	std::map<std::string, GLint> text_to_id;

	GLuint _initShader(GLenum type, const std::string& filename);
	bool _initProgram();

	void _shaderErrors(const GLint shader);
	void _programErrors(const GLint program);
};


