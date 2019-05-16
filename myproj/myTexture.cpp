#include "myTexture.h"

#include <map>
#include <tuple>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include "myShader.h"
#include "helperFunctions.h"

myTexture::myTexture()
{
	texture_id = 0;
	texture_type = GL_TEXTURE_2D;
}

myTexture::myTexture(GLenum type)
{
	texture_id = 0;
	texture_type = type;
}

myTexture::myTexture(std::string filename)
{
	texture_id = 0;
	texture_type = GL_TEXTURE_2D;
	readTexture_2D(filename);
}

myTexture::myTexture(std::vector<std::string> & filenames)
{
	texture_id = 0;
	texture_type = GL_TEXTURE_CUBE_MAP;
	readTexture_cubemap(filenames);
}


myTexture::~myTexture()
{
	glDeleteTextures(1, &texture_id);
}


bool myTexture::readTexture_2D(std::string filename)
{
	int size; GLubyte* mytexture;

	glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
	mytexture = stbi_load(filename.c_str(), &width, &height, &size, 0);

	std::map<int, std::tuple<GLint, GLenum>> map;

	map[1] = std::make_tuple(GL_RED, GL_R16F);
	map[3] = std::make_tuple(GL_RGB, GL_RGB16F);
	map[4] = std::make_tuple(GL_RGBA, GL_RGBA16F);

	std::tie(texFormat, internalformat) = map[size];

	glTextureStorage2D(texture_id, 5, internalformat, width, height);
	glTextureSubImage2D(texture_id, 0, 0, 0, width, height, texFormat, GL_UNSIGNED_BYTE, mytexture);
	
	configTexture(texture_id);

	stbi_image_free(mytexture);
	return true;
}

bool myTexture::readTexture_HDR(std::string filename) {

	std::map<int, std::tuple<GLint, GLenum>> map;

	map[1] = std::make_tuple(GL_RED, GL_R16F);
	map[3] = std::make_tuple(GL_RGB, GL_RGB16F);
	map[4] = std::make_tuple(GL_RGBA, GL_RGBA16F);

	stbi_set_flip_vertically_on_load(true); int size;
	auto textureData = stbi_loadf(filename.c_str(), &width, &height, &size, 3);
	stbi_set_flip_vertically_on_load(false);

	std::tie(texFormat, internalformat) = map[size];

	glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
	glTextureStorage2D(texture_id, 1, internalformat, width, height);
	glTextureSubImage2D(texture_id, 0, 0, 0, width, height, texFormat, GL_FLOAT, textureData);

	configTexture(texture_id);

	stbi_image_free(textureData);
	return true;
}

void  myTexture::configTexture(GLuint theTexture) {

	GLfloat anisoFilterLevel;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoFilterLevel);
	glTextureParameteri(theTexture, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoFilterLevel);

	glTextureParameteri(theTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(theTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(theTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(theTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateTextureMipmap(theTexture);
}

void myTexture::readTexture_cubemap(std::vector<std::string>& filenames)
{
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };

	for (Face f : {LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK})
	{
		int size, width, height;
		GLubyte *mytexture = stbi_load(filenames[f].c_str(), &width, &height, &size, 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mytexture);
		delete[] mytexture;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void myTexture::bind(myShader *shader, std::string name, GLuint texture_offset )
{
	glBindTextureUnit(texture_offset, texture_id);
	shader->setUniform(name, static_cast<int>(texture_offset));
}
