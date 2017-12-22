#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    
#include <string>
#include <vector>
#include <sstream>

#include <GL/glew.h>    
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
	output << "Start OpenGL Information.\n";

	vector<tuple<int, string>> string_params =
	{
		make_tuple(GL_VENDOR, "Vendor"), 
		make_tuple(GL_RENDERER, "Renderer"),
		make_tuple(GL_VERSION, "Version")
	};

	for (size_t i = 0; i < string_params.size(); i++)
	{
		string res = (char *)glGetString(get<0>(string_params[i]));
		if (!res.empty()) output << "\t" << get<1>(string_params[i]) << ": "<< res << endl;
	}

	{
		stringstream res ((char*)glGetString(GL_EXTENSIONS));
		string s;
		output << "\tExtensions: \n";
		while (res >> s)
			output << "\t\t" << s << endl;
	}

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
		int res; 
		glGetIntegerv(get<0>(int_params[i]), &res);
		output << "\t" << get<1>(int_params[i]) << ": " << res << endl;
	}

	output << "End OpenGL Information.\n";


	if (toprint) cout << output.str(); 


	/*
	// get renderer string
	str = (char*)glGetString(GL_RENDERER);
	if (str) this->renderer = str;                // check NULL return value
	else return false;

	// get version string
	str = (char*)glGetString(GL_VERSION);
	if (str) this->version = str;                 // check NULL return value
	else return false;

	// get all extensions as a string
	str = (char*)glGetString(GL_EXTENSIONS);

	// split extensions
	if (str)
	{
		tok = strtok((char*)str, " ");
		while (tok)
		{
			this->extensions.push_back(tok);    // put a extension into struct
			tok = strtok(0, " ");               // next token
		}
	}
	else
	{
		return false;
	}

	// sort extension by alphabetical order
	std::sort(this->extensions.begin(), this->extensions.end());

	// get number of color bits
	glGetIntegerv(GL_RED_BITS, &this->redBits);
	glGetIntegerv(GL_GREEN_BITS, &this->greenBits);
	glGetIntegerv(GL_BLUE_BITS, &this->blueBits);
	glGetIntegerv(GL_ALPHA_BITS, &this->alphaBits);

	// get depth bits
	glGetIntegerv(GL_DEPTH_BITS, &this->depthBits);

	// get stecil bits
	glGetIntegerv(GL_STENCIL_BITS, &this->stencilBits);

	// get max number of lights allowed
	glGetIntegerv(GL_MAX_LIGHTS, &this->maxLights);

	// get max texture resolution
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->maxTextureSize);

	// get max number of clipping planes
	glGetIntegerv(GL_MAX_CLIP_PLANES, &this->maxClipPlanes);

	// get max modelview and projection matrix stacks
	glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &this->maxModelViewStacks);
	glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &this->maxProjectionStacks);
	glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &this->maxAttribStacks);

	// get max texture stacks
	glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &this->maxTextureStacks);

	*/
}