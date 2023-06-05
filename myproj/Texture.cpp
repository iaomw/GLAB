#include "Texture.h"
#include <gl/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sstream>
#include <iostream>

Texture::Texture() : Texture(GL_TEXTURE_2D) {}

Texture::Texture(GLenum type)
{
	texture_id = 0;
	texture_type = type;
}

Texture::Texture(const std::string& filename, const int channel)
{
	texture_id = 0;
	texture_type = GL_TEXTURE_2D;
	readTexture2D(filename, channel);
}

Texture::Texture(std::vector<std::string>& filenames)
{
	texture_id = 0;
	texture_type = GL_TEXTURE_CUBE_MAP;
	readTextureCube(filenames);
}

Texture::~Texture()
{
	if (texture_handle) {
		glMakeTextureHandleNonResidentARB(texture_handle);
		texture_handle = 0;
	}

	printf("Texture %u has been destoryed \n", texture_id);

	glDeleteTextures(1, &texture_id);
	texture_id = 0;
}

GLuint64 Texture::getHandle() {

	if (texture_handle == 0) {	
		texture_handle = glGetTextureHandleARB(texture_id);
		glMakeTextureHandleResidentARB(texture_handle);
	}

	return texture_handle;
}

inline void Texture::bind() {
	glBindTexture(texture_type, texture_id);
}

inline void Texture::unbind() {
	glBindTexture(texture_type, 0);
}

GLuint getNumMipMapLevels2D(GLuint w, GLuint h)
{
	GLuint levels = 1;
	while ((w | h) >> levels)
		levels += 1;
	return levels;
}

void Texture::empty() {
	glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);

	width = 1, height = 1;
	auto textureFormat = GL_RGBA;
	auto internalFormat = GL_RGBA16F;

	std::vector<GLubyte> emptyData(4, 0);
	glTextureStorage2D(texture_id, 1, internalFormat, width, height);
	glTextureSubImage2D(texture_id, 0, 0, 0, width, height, textureFormat, GL_UNSIGNED_BYTE, &emptyData[0]);

	//configTexture(texture_id);
}

bool Texture::readTexture2D(const std::string& filename, const int channel)
{
	int comp=0; GLubyte* theTexture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);

	if (channel > 1) { comp = 4;}

	theTexture = stbi_load(filename.c_str(), &width, &height, &comp, comp);

	std::cout << filename << std::endl;
	std::cout << "This image width: " << width << " height " << height << " channels " << comp << std::endl;

	if (channel > 1) {
		comp = 4;
	} else {
		comp = 1;
	}

	static const std::map<int, std::tuple<GLenum, GLenum>> map {
		{1, {GL_RED,     GL_R8}},
		{2, {GL_RG,     GL_RG8}}, // Will lead to crash in some case
		{3, {GL_RGB,   GL_RGB8}},
		{4, {GL_RGBA, GL_RGBA8}}
	};

	std::tie(textureFormat, internalFormat) = map.at(comp);
	auto mip = getNumMipMapLevels2D(width, height);

	glTextureStorage2D(texture_id, mip, internalFormat, width, height);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTextureSubImage2D(texture_id, 0, 0, 0, width, height, textureFormat, GL_UNSIGNED_BYTE, theTexture);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	configTexture(texture_id);
	stbi_image_free(theTexture);
	return true;
}

bool Texture::readTextureHDR(const std::string& filename) {

	std::map<int, std::tuple<GLint, GLenum>> map;

	map[1] = std::make_tuple(GL_RED, GL_R16F);
	map[3] = std::make_tuple(GL_RGB, GL_RGB16F);
	map[4] = std::make_tuple(GL_RGBA, GL_RGBA16F);

	stbi_set_flip_vertically_on_load(true); int size;
	auto textureData = stbi_loadf(filename.c_str(), &width, &height, &size, 0);
	stbi_set_flip_vertically_on_load(false);

	std::tie(textureFormat, internalFormat) = map[size];

	glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
	glTextureStorage2D(texture_id, 1, internalFormat, width, height);
	glTextureSubImage2D(texture_id, 0, 0, 0, width, height, textureFormat, GL_FLOAT, textureData);

	configTexture(texture_id);
	stbi_image_free(textureData);
	return true;
}

void Texture::configTexture(GLuint theTexture) {

	GLfloat anisoFilterLevel;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoFilterLevel);
	glTextureParameteri(theTexture, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoFilterLevel);

	glTextureParameteri(theTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(theTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(theTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(theTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateTextureMipmap(theTexture);

	// GLuint sampler;
	// glGenSamplers(1, &sampler);

	// glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// texture_handle = glGetTextureSamplerHandleARB(texture_id, sampler);

	texture_handle = glGetTextureHandleARB(texture_id);
	glMakeTextureHandleResidentARB(texture_handle);
	
}

void Texture::readTextureCube(const std::vector<std::string>& filenames)
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture_id);

	//enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };

	for (auto f : {0, 1, 2, 3, 4, 5})
	{
		int size, width, height;
		GLubyte *theTexture = stbi_load(filenames[f].c_str(), &width, &height, &size, 4);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mytexture);
		glTextureSubImage3D(texture_id, 0, 0, 0, f,
			width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, theTexture);
		
		delete[] theTexture;
	}

	glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
