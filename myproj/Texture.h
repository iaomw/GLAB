#pragma once
#include <map>
#include <vector>
#include <string>

#include <GL/glew.h>

enum class Texture_Type {

	colortex, normaltex, cubetex, depthtex,

	gAlbedo, gNormal, gEnv,

	gExtra, gBloom, gSSSS,

	Unknow
};

class Texture
{
public:
	GLuint texture_id = 0;
	GLuint64 texture_handle = 0;
		
	GLenum texture_type = 0;
	GLenum textureFormat = 0;
	GLenum internalFormat = 0;

	int width=0, height=0;

	Texture();
	Texture(GLenum type);
	Texture(const std::string& filename);
	Texture(std::vector<std::string>& filenames);
	~Texture();

	GLuint64 getHandle();

	inline void bind();
	inline void unbind();

	void empty();
	bool readTexture2D(const std::string& filename);
	bool readTextureHDR(const std::string& filename);
	void readTextureCube(const std::vector<std::string>& filenames);

private:
	void configTexture(GLuint theTexture);
};

