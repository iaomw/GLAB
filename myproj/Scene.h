#pragma once

#include "Shader.h"
//#include "LightList.h"

#include "MeshPack.h"
#include "Camera.h"
#include <unordered_map>

class Scene
{
public:
	std::unordered_multimap<std::string, size_t> byname;
	std::unique_ptr<LightList> lightList;
	std::vector<MeshPack*> all_objects;
	
	Scene();
	~Scene();

	void addObjects(MeshPack *, const std::string& name = "default");
	MeshPack * & operator[](const std::string& name);
	MeshPack * & operator[](const unsigned int index);

	size_t size() const;

	float closestObject(glm::vec3 ray, glm::vec3 camera_eye, MeshPack * & picked_object, size_t & picked_triangle_index);
};
