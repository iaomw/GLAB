#pragma once
#include "myLights.h"
#include "myShader.h"

#include "myObject.h"
#include "myCamera.h"
#include <unordered_map>

class myScene
{
public:
	std::unordered_multimap<std::string, size_t> byname;
	std::vector<myObject *> all_objects;
	myLights *lights;
    
	myScene();
	~myScene();

	void addObjects(myObject *, const std::string& name = "default");
	myObject * & operator[](const std::string& name);
	myObject * & operator[](const unsigned int index);

	size_t size() const;

	float closestObject(glm::vec3 ray, glm::vec3 camera_eye, myObject * & picked_object, size_t & picked_triangle_index);
};
