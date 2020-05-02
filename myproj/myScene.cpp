#include "myScene.h"
#include <iostream>

myScene::myScene()
{
	lights = nullptr;
}

myScene::~myScene()
{
	for (unsigned int i = 0;i < all_objects.size();i++)
		if (all_objects[i] != nullptr) delete all_objects[i];
	if (lights != nullptr) delete lights;
}

void myScene::addObjects(myObject* obj, const std::string& name)
{
	if (obj == nullptr) 
	{
		std::cout << "Error: inserting nullptr for " << name << std::endl;
		return;
	}

	byname.emplace(name, all_objects.size());
	all_objects.push_back(obj);
}

myObject* & myScene::operator[](const std::string& name)
{
	size_t num = byname.count(name);
	assert(num != 0);
	
	if (num > 1)
		std::cout << "Warning: multiple objects with name " << name << std::endl;
	
	return all_objects[byname.find(name)->second];
}

myObject*& myScene::operator[](const unsigned int index)
{
	assert(index >= 0 && index < all_objects.size());

	return all_objects[index];
}

size_t myScene::size() const
{
	return all_objects.size();
}

float myScene::closestObject(glm::vec3 ray, glm::vec3 origin, myObject * & picked_object, size_t & picked_triangle_index)
{
	float min_t = std::numeric_limits<float>::max();
	
	picked_object = nullptr;
	picked_triangle_index = -1;
	int min_index = -1;

	for (unsigned int i=0;i<all_objects.size();i++)
	{
		myObject *objs = all_objects[i];

		size_t curr_picked_triangle_index;
		float curr_t = objs->closestTriangle(ray, origin, curr_picked_triangle_index);

		if (curr_t >= 0 && curr_t < min_t)
		{
			min_t = curr_t;
			picked_triangle_index = curr_picked_triangle_index;
			picked_object = objs;
			min_index = i;
		}
	}

	if (min_index == -1)
	{
		std::cout << "No object in the scene intersected!" << std::endl;
		return -1;
	}

	std::unordered_multimap<std::string, size_t>::iterator it;
	for (it = byname.begin(); it != byname.end(); ++it)
		if (it->second == min_index)
			break;

	std::cout << "Closest object is " << it->first << std::endl;
	return min_t;
}
