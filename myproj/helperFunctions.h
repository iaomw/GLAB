#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    
#include <string>
#include <vector>
#include <sstream>

#include <GL/glew.h>  
#include <SDL2/SDL.h>

#include <iostream>
#include <tuple>

#include "errors.h"

using namespace std;

//rotates inputvec around rotation_axis by angle theta in three dimensions.
static void rotate(glm::vec3 & inputvec, glm::vec3 rotation_axis, float theta, bool tonormalize = false)
{
	const float cos_theta = cos(theta);
	const float dot = glm::dot(inputvec, rotation_axis);
	glm::vec3 cross = glm::cross(inputvec, rotation_axis);

	inputvec.x *= cos_theta; inputvec.y *= cos_theta; inputvec.z *= cos_theta;
	inputvec.x += rotation_axis.x * dot * static_cast<float>(1.0 - cos_theta);
	inputvec.y += rotation_axis.y * dot * static_cast<float>(1.0 - cos_theta);
	inputvec.z += rotation_axis.z * dot * static_cast<float>(1.0 - cos_theta);

	inputvec.x -= cross.x * sin(theta);
	inputvec.y -= cross.y * sin(theta);
	inputvec.z -= cross.z * sin(theta);

	if (tonormalize) inputvec = glm::normalize(inputvec);
}

//Reads a .PPM image file and returns it in a byte array together with its image width and height.
static GLubyte * readPPMfile(const char *filename, int & width, int & height)
{
	FILE *inFile;
	char buffer[100];
	unsigned char c;
	int maxVal;

	GLubyte *mytexture;
	int pixelsize;

	if ((inFile = fopen(filename, "rb")) == nullptr) {
		return nullptr;
	}

	//Read file type identifier (magic number)
	fgets(buffer, sizeof(buffer), inFile);
	if ((buffer[0] != 'P') || (buffer[1] != '6')) {
		fprintf(stderr, "not a binary ppm file %s\n", filename);
		return nullptr;
	}

	if (buffer[2] == 'A')
		pixelsize = 4;
	else
		pixelsize = 3;

	//Read image size
	do fgets(buffer, sizeof(buffer), inFile);
	while (buffer[0] == '#');
	sscanf(buffer, "%d %d", &width, &height);

	//Read maximum pixel value (usually 255)
	do fgets(buffer, sizeof(buffer), inFile);
	while (buffer[0] == '#');
	sscanf(buffer, "%d", &maxVal);

	//Allocate RGBA texture buffer
	int memSize = width * height * 4 * sizeof(GLubyte);
	mytexture = new GLubyte[memSize];

	// read RGB data and set alpha value
	for (int i = 0; i < memSize; i++) {
		if ((i % 4) < 3 || pixelsize == 4) {
			c = fgetc(inFile);
			mytexture[i] = static_cast<GLubyte>(c);
		}
		else mytexture[i] = static_cast<GLubyte>(255); //Set alpha to opaque
	}
	fclose(inFile);
	return mytexture;
}

static void parseObjFace(std::string t, unsigned int & vertex_index, unsigned int & texture_index, unsigned int & normal_index)
{
	vertex_index = texture_index = normal_index = 0;

	size_t first_slash = t.find("/");
	size_t second_slash = t.find_last_of("/");

	if (first_slash == std::string::npos)
		vertex_index = atoi(t.c_str()) - 1;
	else
	{
		vertex_index = atoi(t.substr(0, first_slash).c_str()) - 1;
		if (second_slash == first_slash)
			texture_index = atoi(t.substr(first_slash + 1, t.size()).c_str()) - 1;
		else
		{
			normal_index = atoi(t.substr(second_slash + 1, t.size()).c_str()) - 1;
			if (second_slash > first_slash + 1)
				texture_index = atoi(t.substr(first_slash + 1, second_slash).c_str()) - 1;
		}
	}
}

static void checkOpenGLInfo(bool toprint = false)
{
	myassert(glBindBuffer != 0);
	myassert(glBindVertexArray != 0);
	myassert(glBufferData != 0);
	myassert(glClear != 0);
	myassert(glClearColor != 0);
	myassert(glCullFace != 0);
	myassert(glDepthFunc != 0);
	myassert(glDeleteBuffers != 0);
	myassert(glDeleteVertexArrays != 0);
	myassert(glDisableVertexAttribArray != 0);
	myassert(glDrawArrays != 0);
	myassert(glEnable != 0);
	myassert(glGenVertexArrays != 0);
	myassert(glGenBuffers != 0);
	myassert(glUseProgram != 0);
	myassert(glUniformMatrix4fv != 0);
	myassert(glVertexAttribPointer != 0);
	myassert(glViewport != 0);

	stringstream output;
	output << endl <<"Start OpenGL Information.\n\n";

	vector<tuple<int, string>> string_params =
	{
		make_tuple(GL_VENDOR, "Vendor"),
		make_tuple(GL_RENDERER, "Renderer"),
		make_tuple(GL_VERSION, "Version")
	};

	for (size_t i = 0; i < string_params.size(); i++)
	{
		string res = (char*)glGetString(get<0>(string_params[i]));
		if (!res.empty()) output << "\t" << get<1>(string_params[i]) << ": " << res << endl;
	};  output << endl;

	GLint n, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (i = 0; i < n; i++) {
		output << "\t" << glGetStringi(GL_EXTENSIONS, i) << endl;
	}; output << endl;

	vector<tuple<int, string>> int_params =
	{
		make_tuple(GL_RED_BITS, "Red bits"),
		make_tuple(GL_GREEN_BITS, "Green bits"),
		make_tuple(GL_BLUE_BITS, "Blue bits"),
		make_tuple(GL_ALPHA_BITS, "Alpha bits"),
		make_tuple(GL_DEPTH_BITS, "Depth bits"),
		make_tuple(GL_STENCIL_BITS, "Stencil bits"),
		make_tuple(GL_MAX_TEXTURE_SIZE, "Max texture size"),
		make_tuple(GL_MAX_CLIP_PLANES, "Max clip planes")
	};

	for (size_t i = 0; i < int_params.size(); i++)
	{
		GLint name;
		glGetIntegerv(get<0>(int_params[i]), &name);
		output << "\t" << get<1>(int_params[i]) << ": " << name << endl;
	}; output << endl;

	output << "End OpenGL Information.\n\n";
	if (toprint) cout << output.str();
}

static void SDL_GetDisplayDPIRatio(int displayIndex, float* dpi, float* defaultDpi)
{
	const float kSysDefaultDpi =
#ifdef __APPLE__
		72.0f;
#elif defined(_WIN32)
		96.0f;
#else
		static_assert(false, "No system default DPI set for this platform.");
#endif

	if (SDL_GetDisplayDPI(displayIndex, NULL, dpi, NULL) != 0)
	{
		// Failed to get DPI, so just return the default value.
		if (dpi)* dpi = kSysDefaultDpi;
	}

	if (defaultDpi)* defaultDpi = kSysDefaultDpi;
}