#pragma once
#include <unordered_map>
#include "myShader.h"

class myShaders
{
public:
	std::unordered_map<std::string, size_t> byname;
	std::vector<myShader *> all_shaders;

	myShaders();
	~myShaders();

	void addShader(myShader *, std::string name);
	myShader * & operator[](const std::string name);
	myShader * & operator[](const unsigned int index);

	size_t size() const;
};

