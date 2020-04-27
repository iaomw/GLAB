#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>

class myShader;

class myTexture
{
public:
	GLuint texture_id;
	GLenum texture_type;
		
	GLint textureFormat;
	GLenum internalFormat;
	int width=0, height=0;

	myTexture();
	myTexture(GLenum type);
	myTexture(const std::string& filename);
	myTexture(std::vector<std::string>& filenames);
	~myTexture();

	bool readTexture2D(const std::string& filename);
	bool readTextureHDR(const std::string& filename);
	void readTextureCube(const std::vector<std::string>& filenames);

	void bind(myShader *shader, std::string_view name, GLuint texture_offset = 0);

private:
	void configTexture(GLuint theTexture);
};

