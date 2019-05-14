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
		
	GLint texFormat;
	int width, height;

	myTexture();
	myTexture(GLenum type);
	myTexture(std::string filename);
	myTexture(std::vector<std::string> & filenames);
	~myTexture();
	
	bool readTexture_2D(std::string filename);
	void readTexture_cubemap(std::vector<std::string> & filenames);

	void bind(myShader *shader, std::string name, GLuint texture_offset = 0);
};

