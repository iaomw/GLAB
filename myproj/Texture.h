#pragma once

#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>

class Texture
{
public:
	GLuint texture_id;
	GLenum texture_type;
		
	GLenum textureFormat = 0;
	GLenum internalFormat = 0;
	int width=0, height=0;

	Texture();
	Texture(GLenum type);
	Texture(const std::string& filename);
	Texture(std::vector<std::string>& filenames);
	~Texture();

	void empty();
	bool readTexture2D(const std::string& filename);
	bool readTextureHDR(const std::string& filename);
	void readTextureCube(const std::vector<std::string>& filenames);

private:
	void configTexture(GLuint theTexture);
};

