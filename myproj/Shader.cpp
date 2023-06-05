#include "Shader.h"

#include <fstream>
#include <iostream>
#include <filesystem>

const static std::string shaderURI = "shaders/";

Shader::Shader(const std::string& key) {

	text_to_id.clear();

	auto vs = shaderURI + key + ".vs.glsl";
	auto gs = shaderURI + key + ".gs.glsl";
	auto fs = shaderURI + key + ".fs.glsl";

	if (std::filesystem::exists(vs)) {
		vertex_shader = _initShader(GL_VERTEX_SHADER, vs);
	}

	if (std::filesystem::exists(gs)) {
		geometry_shader = _initShader(GL_GEOMETRY_SHADER, gs);
	}

	if (std::filesystem::exists(fs)) {
		fragment_shader = _initShader(GL_FRAGMENT_SHADER, fs);
	}

	if (!_initProgram())
		std::cout << "Error: shader not initialized properly.\n";
}

Shader::Shader(const std::string& file_vertexshader, const std::string& file_fragmentshader)
{
	text_to_id.clear();

	vertex_shader = _initShader(GL_VERTEX_SHADER, file_vertexshader);
	fragment_shader = _initShader(GL_FRAGMENT_SHADER, file_fragmentshader);
	
	if (!_initProgram())
		std::cout << "Error: shader not initialized properly.\n";
}

Shader::Shader(const std::string& file_vertexshader, const std::string& file_geometryshader, const std::string& file_fragmentshader)
{
	text_to_id.clear();

	vertex_shader = _initShader(GL_VERTEX_SHADER, file_vertexshader);
	geometry_shader = _initShader(GL_GEOMETRY_SHADER, file_geometryshader);
	fragment_shader = _initShader(GL_FRAGMENT_SHADER, file_fragmentshader);

	if (!_initProgram())
		std::cout << "Error: shader not initialized properly.\n";
}

Shader::~Shader()
{
	clear();
}

GLuint Shader::_initShader(GLenum type, const std::string& filename)
{
	std::ifstream fin(filename);

	if (!fin)
	{
		std::cerr << "Unable to Open File " << filename << "\n";
		throw 2;
	}
	std::string shadertext = std::string((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));
	const GLchar* shadertext_cstr = shadertext.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shadertext_cstr, NULL);

	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		_shaderErrors(shader);
		throw 3;
	}
	return shader;
}

bool Shader::_initProgram()
{
	shaderprogram = glCreateProgram();
	if (vertex_shader != 0) {
		glAttachShader(shaderprogram, vertex_shader);
	}
	if (geometry_shader != 0) {
		glAttachShader(shaderprogram, geometry_shader);
	}
	if (fragment_shader != 0) {
		glAttachShader(shaderprogram, fragment_shader);
	}
	glLinkProgram(shaderprogram);

	GLint linked;
	glGetProgramiv(shaderprogram, GL_LINK_STATUS, &linked);
	if (linked) glUseProgram(shaderprogram);
	else {
		_programErrors(shaderprogram);
		return false;
	}
	return true;
}

void Shader::_programErrors(const GLint program) {
	GLint length;
	GLchar * log;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	log = new GLchar[(size_t)1 + length];
	glGetProgramInfoLog(program, length, &length, log);
	std::cout << "Compile Error, Log Below\n" << log << "\n";
	delete[] log;
}

void Shader::_shaderErrors(const GLint shader) {
	GLint length;
	GLchar * log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	log = new GLchar[(size_t)1 + length];
	glGetShaderInfoLog(shader, length, &length, log);
	std::cout << "Compile Error, Log Below\n" << log << "\n";
	delete[] log;
}

void Shader::clear()
{
	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shaderprogram);
}

void Shader::start() const
{
	glUseProgram(shaderprogram);
}

void Shader::stop() const
{
	glUseProgram(0);
}

GLint Shader::getUniformLocation(const std::string& text) 
{
	if (text_to_id.find(text) == text_to_id.end())
	{
		int location = glGetUniformLocation(shaderprogram, text.c_str());
		if (location == -1)
		{
			//cerr << "Error: unable to get location of variable with name: " << text << endl;
			text_to_id[text] = -1;
			return -1;
		}
		else text_to_id[text] = location;
	}

	return text_to_id[text];
}

void Shader::setUniform(const std::string& name, glm::mat4 & mat)
{
	auto location = getUniformLocation(name);
	if (-1 == location) { return; }
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setUniform(const std::string& name, glm::mat3 &mat)
{
	auto location = getUniformLocation(name);
	if (-1 == location) { return; }
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setUniform(const std::string& name, float val)
{
	auto location = getUniformLocation(name);
	if (-1 == location) { return; }
	glUniform1f(location, val);
}

void Shader::setUniform(const std::string& name, int val)
{
	auto location = getUniformLocation(name);
	if (-1 == location) { return; }
	glUniform1i(location, val);
}

void Shader::setUniform(const std::string& name, glm::vec2 vec)
{
	auto location = getUniformLocation(name);
	if (-1 == location) { return; }
	glUniform2fv(location, 1, glm::value_ptr(vec));
}

void Shader::setUniform(const std::string& name, glm::vec3& vec)
{
	auto location = getUniformLocation(name);
	if (-1 == location) { return; }
	glUniform3fv(location, 1, glm::value_ptr(vec));
}

void Shader::setUniform(const std::string& name, glm::vec4& vec)
{
	auto location = getUniformLocation(name);
	if (-1 == location) { return; }
	glUniform4fv(location, 1, glm::value_ptr(vec));
}

void Shader::setUniform(const std::string& name, std::vector<glm::vec3>& input_array)
{
	for (unsigned int i = 0; i < input_array.size(); ++i) {

		auto location = getUniformLocation(name + "[" + std::to_string(i) + "]");
		if (-1 == location) { continue; }
		glUniform3fv(location, 1, &input_array[i][0]);
	}
}

void Shader::setUniform(const std::string& name, std::vector<glm::vec4>& input_array)
{
	//glUniform4fv(getUniformLocation(name), input_array.size(), &input_array[0][0]);
	for (unsigned int i = 0; i < input_array.size(); ++i) {

		auto location = getUniformLocation(name + "[" + std::to_string(i) + "]");
		if (-1 == location) { return; }
		glUniform4fv(location, 1, &input_array[i][0]);
	}
}

void Shader::setTex(const std::string& name, GLuint texture_id, GLuint texture_offset)
{
	glBindTextureUnit(texture_offset, texture_id);
	this->setUniform(name, static_cast<int>(texture_offset));
}
