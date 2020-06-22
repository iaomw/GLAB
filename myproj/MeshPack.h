#pragma once

#include <fstream>
#include <unordered_map>

#define GLM_FORCE_AVX
#include <glm/glm.hpp>

#include "VAO.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "MeshPart.h"

class MeshPack
{
public:
	MeshPack();
	~MeshPack();
	void clear();

	bool readObjects(const std::string& filename, bool individualvertices_per_face = true, bool tonormalize = false, bool subgroup = false);

	void createVAO();
	void normalize();
	void computeNormals();

	glm::mat4 normalMatrix(glm::mat4&, glm::mat4&);

	void displayObjects(std::unique_ptr<Shader> const& shader, glm::mat4&);
	
	glm::vec3 closestVertex(glm::vec3 ray, glm::vec3 starting_point);
	float closestTriangle(glm::vec3 ray, glm::vec3 origin, size_t& picked_triangle);
	float closestTriangle(glm::vec3 ray, glm::vec3 origin, size_t & picked_triangle, MeshPart * & picked_object);
	
	void scale(float x, float y, float z);
	void translate(float x, float y, float z);
	
	void rotate(float axis_x, float axis_y, float axis_z, float angle);

	std::vector<glm::mat4> modelMatrixList;

	void batch();

	void scale(glm::vec3&);
	void translate(glm::vec3&);
	void rotate(glm::vec3& axis, float angle);

	void computeTexturecoordinates_plane();
	void computeTexturecoordinates_sphere();
	void computeTexturecoordinates_cylinder();
	
	void computeTangents();
	
	void setTexture(Texture*, Texture_Type);
	void clearTexture();
	
	std::unique_ptr<VAO> vao;

	std::vector<glm::ivec3> indices;
	std::vector<glm::vec3> vertices;

	std::vector<glm::vec3> f_normals;
	std::vector<glm::vec3> v_normals;
	
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec2> texcoords;
	
	std::string name;
	glm::mat4 model_matrix;
	std::unordered_multimap<std::string, MeshPart*> children;
};

