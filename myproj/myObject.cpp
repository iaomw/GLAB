#include "myObject.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "helperFunctions.h"
#include "errors.h"

myObject::myObject()
{
	model_matrix = glm::mat4(1.0f);
	vao = nullptr;
	name = "default";
}

myObject::~myObject()
{
	clear();
}

void myObject::clear()
{
	if (vao) delete vao;

	for (std::unordered_multimap<std::string, mySubObject *>::iterator it = subObjects.begin(); it != subObjects.end(); ++it)
		delete it->second;
	subObjects.clear();

	std::vector<glm::vec3> empty_f; 
	vertices.swap(empty_f);
	vertex_normals.swap(empty_f);

	std::vector<glm::ivec3> empty_i; 
	indices.swap(empty_i);
}

void myObject::readMaterials(const std::string& mtlfilename, std::unordered_map<std::string, myMaterial *> & materials, std::unordered_map<std::string, myTexture *> & textures)
{
	std::ifstream mtlfin(mtlfilename);

	//if (!mtlfin.is_open()) // very slow in some case
	if(!mtlfin.good())
	{
		std::cout << "Error! Unable to open mtl file.\n";
		return;
	}

	std::string v;
	myMaterial *curr_mat = nullptr;

	while (mtlfin >> v)
	{
		if (v == "newmtl")
		{
			if (curr_mat != nullptr)
				materials.emplace(curr_mat->mat_name, curr_mat);
			curr_mat = new myMaterial();
			mtlfin >> curr_mat->mat_name;
		}
		else if (v == "Ns") mtlfin >> curr_mat->specular_coefficient;
		else if (v == "Ka") mtlfin >> curr_mat->ka.r >> curr_mat->ka.g >> curr_mat->ka.b;
		else if (v == "Kd") mtlfin >> curr_mat->kd.r >> curr_mat->kd.g >> curr_mat->kd.b;
		else if (v == "Ks") mtlfin >> curr_mat->ks.r >> curr_mat->ks.g >> curr_mat->ks.b;
		else if (v == "map_Kd") 
		{
			std::string t;
			mtlfin >> t;
			size_t prefix = mtlfilename.find_last_of("/");
			textures.emplace(curr_mat->mat_name, new myTexture(mtlfilename.substr(0, prefix) + "/" + t) );
		}
	}
	mtlfin.close();
}

bool myObject::readObjects(const std::string& filename, bool allow_duplication, bool tonormalize, bool subgroup)
{
	clear();

	std::string s, t;
	std::string mtlfilename;

	std::ifstream fin(filename);
	if (!fin.is_open()) return false;

	size_t curr_start = 0, curr_end;
	std::string curr_name = "noname";
	myMaterial *curr_mat = nullptr;
	myTexture *curr_texture = nullptr;

	std::unordered_map<std::string, myMaterial *> materials;
	std::unordered_map<std::string, myTexture *> textures;

	std::vector<glm::vec3> tmp_vertices;
	std::vector<glm::vec3> tmp_normals;
	std::vector<glm::vec2> tmp_coords;

	std::unordered_map<glm::vec3, uint32_t> cached;

	unsigned int vertex_idx_a, texture_idx_a, normal_idx_a;
	unsigned int vertex_idx_b, texture_idx_b, normal_idx_b;
	unsigned int vertex_idx_c, texture_idx_c, normal_idx_c;
	
	while (getline(fin, s))
	{
		std::stringstream myline(s);
		myline >> t;
		if (t == "g" || t == "o" || t == "s")
		{
			curr_end = indices.size();
			mySubObject *o = new mySubObject(curr_mat, curr_start, curr_end, curr_name);
			//o->setTexture(curr_texture, Texture_Type::colortex);
			subObjects.emplace(curr_name, o);

			curr_start = curr_end;
			myline >> curr_name;
		}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			tmp_vertices.push_back(glm::vec3(x, y, z));
		}
		else if (t == "vn")
		{
			float x, y, z;
			myline >> x >> y >> z;
			tmp_normals.push_back(glm::vec3(x, y, z));
		}
		else if (t == "vt")
		{
			float _s, _t;
			myline >> _s >> _t;
			tmp_coords.push_back(glm::vec2(_s, 1.0f - _t));
		}
		else if (t == "mtllib")
		{
			myline >> mtlfilename;
			size_t prefix = filename.find_last_of("/");
			if (prefix != std::string::npos)
				mtlfilename = (filename.substr(0, prefix)).c_str() + std::string("/") + mtlfilename;
			//readMaterials(mtlfilename, materials, textures);
		}
		else if (t == "usemtl")
		{
			curr_end = indices.size();
			mySubObject *o = new mySubObject(curr_mat, curr_start, curr_end, curr_name);
			//o->setTexture(curr_texture, Texture_Type::colortex);
			subObjects.emplace(curr_name, o);

			curr_start = curr_end;

			std::string u;
			myline >> u;
			curr_name = u;

			if (materials.count(u) != 0)
				curr_mat = materials[u];
			else curr_mat = nullptr;

			if (textures.count(u) != 0)
				curr_texture = textures[u];
			else curr_texture = nullptr;
		}
		else if (t == "f")
		{
			myline >> t; 
			parseObjFace(t, vertex_idx_a, texture_idx_a, normal_idx_a);
	
			myline >> t; 
			parseObjFace(t, vertex_idx_b, texture_idx_b, normal_idx_b);

			while (myline >> t)
			{
				parseObjFace(t, vertex_idx_c, texture_idx_c, normal_idx_c);

				if (allow_duplication) 
				{	// duplicate
					auto transfer = [&](uint32_t v_idx, uint32_t t_idx, uint32_t n_idx) -> uint32_t {

						auto index = vertices.size();
						vertices.push_back(tmp_vertices[v_idx]);

						if (n_idx < tmp_normals.size())
							vertex_normals.push_back(tmp_normals[n_idx]);
						else vertex_normals.push_back(glm::vec3(0, 0, 0));

						if (t_idx < tmp_coords.size())
							texturecoords.push_back(tmp_coords[t_idx]);
						else texturecoords.push_back(glm::vec2(0, 0));

						return index;
					};

					auto cached_a = transfer(vertex_idx_a, texture_idx_a, normal_idx_a);
					auto cached_b = transfer(vertex_idx_b, texture_idx_b, normal_idx_b);
					auto cached_c = transfer(vertex_idx_c, texture_idx_c, normal_idx_c);

					indices.push_back(glm::ivec3(cached_a, cached_b, cached_c));
				}
				else {

					auto cacheCheck = [&](uint32_t v_idx, uint32_t t_idx, uint32_t n_idx) -> uint32_t { // ignore normal

						auto tmp_v = tmp_vertices[v_idx];

						if (cached.count(tmp_v) == 0) {
							cached[tmp_v] = vertices.size();
							auto result = vertices.size();
							vertices.push_back(tmp_v);

							if (tmp_normals.size() > 0 && n_idx < tmp_normals.size()) {
								auto tmp_n = tmp_normals[n_idx];
								vertex_normals.push_back(tmp_n);
							}

							if (tmp_coords.size() > 0 && t_idx < tmp_coords.size()) {
								auto tmp_t = tmp_coords[t_idx];
								texturecoords.push_back(tmp_t);
							}

							return result;
						}
						else {
							auto index_v = cached[tmp_v];
							return index_v;
						}
					};

					auto cached_a = cacheCheck(vertex_idx_a, texture_idx_a, normal_idx_a);
					auto cached_b = cacheCheck(vertex_idx_b, texture_idx_b, normal_idx_b);
					auto cached_c = cacheCheck(vertex_idx_c, texture_idx_c, normal_idx_c);

					indices.push_back(glm::ivec3(cached_a, cached_b, cached_c));
				}

				vertex_idx_b = vertex_idx_c;
				normal_idx_b = normal_idx_c;
				texture_idx_b = texture_idx_c;
			}
		}
		s.clear();
		t.clear();
	}

	curr_end = indices.size();

	if (subgroup) {

		mySubObject* o = new mySubObject(curr_mat, curr_start, curr_end, curr_name);
		//o->setTexture(curr_texture, Texture_Type::colortex);
		subObjects.emplace(curr_name, o);
	}
	else {

		mySubObject* o = new mySubObject(curr_mat, 0, indices.size(), curr_name);
		//o->setTexture(curr_texture, Texture_Type::colortex);
			subObjects.clear();
		subObjects.emplace(curr_name, o);
	}

	if (tmp_normals.size() == 0) {
		computeNormals();
	}

	if (tonormalize) normalize();

	return true;
}

void myObject::normalize()
{
	unsigned int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned i = 0; i < vertices.size(); i++) {
		if (vertices[i].x < vertices[tmpxmin].x) tmpxmin = i;
		if (vertices[i].x > vertices[tmpxmax].x) tmpxmax = i;

		if (vertices[i].y < vertices[tmpymin].y) tmpymin = i;
		if (vertices[i].y > vertices[tmpymax].y) tmpymax = i;

		if (vertices[i].z < vertices[tmpzmin].z) tmpzmin = i;
		if (vertices[i].z > vertices[tmpzmax].z) tmpzmax = i;
	}

	float xmin = vertices[tmpxmin].x, xmax = vertices[tmpxmax].x,
		ymin = vertices[tmpymin].y, ymax = vertices[tmpymax].y,
		zmin = vertices[tmpzmin].z, zmax = vertices[tmpzmax].z;

	float scale = ((xmax - xmin) <= (ymax - ymin)) ? (xmax - xmin) : (ymax - ymin);
	scale = (scale >= (zmax - zmin)) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i].x -= (xmax + xmin) / 2;
		vertices[i].y -= (ymax + ymin) / 2;
		vertices[i].z -= (zmax + zmin) / 2;

		vertices[i].x /= scale;
		vertices[i].y /= scale;
		vertices[i].z /= scale;
	}
}

void myObject::computeNormals()
{
	face_normals.assign(indices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
	vertex_normals.assign(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		auto a = vertices[indices[i][1]] - vertices[indices[i][0]];
		auto b = vertices[indices[i][2]] - vertices[indices[i][1]];

		glm::vec3 face_normal = glm::cross(a, b);
		face_normals[i] = face_normal;

		vertex_normals[indices[i][0]] += face_normal;  
		vertex_normals[indices[i][1]] += face_normal;  
		vertex_normals[indices[i][2]] += face_normal;  
	}

	std::unordered_map<glm::vec3, glm::vec3> cached; 

	for (unsigned int i = 0; i < vertices.size(); i++) {
		auto& v = vertices[i];
		auto& n = vertex_normals[i];

		if (cached.count(v) == 0) {
			cached[v] = n ;
		}
		else {
			cached[v] += n;
		}
	}

	for (unsigned int i = 0; i < vertices.size(); i++) {

		auto& v = vertices[i];
		auto& n = cached[v];

		vertex_normals[i] = glm::normalize(n);
	}
}

void myObject::createmyVAO()
{
	delete vao;
	vao = new myVAO();

	if (indices.size()) vao->storeIndices(indices);
	if (vertices.size()) vao->storePositions(vertices, 0);
	if (vertex_normals.size()) vao->storeNormals(vertex_normals, 1);
	if (texturecoords.size()) vao->storeTexturecoordinates(texturecoords, 2);

	if (tangents.size()) vao->storeTangents(tangents, 3);
}

glm::mat3 myObject::normalMatrix(glm::mat4 view_matrix) {
	return glm::transpose(glm::inverse(glm::mat3(view_matrix) * glm::mat3(model_matrix)));
}

void myObject::displayObjects(myShader *shader, glm::mat4 view_matrix)
{
	myassert(vao != nullptr);

	shader->setUniform("mymodel_matrix", model_matrix);
	shader->setUniform("mynormal_matrix", normalMatrix(view_matrix));

	for (std::unordered_multimap<std::string, mySubObject*>::iterator it = subObjects.begin(); it != subObjects.end(); ++it) {
		it->second->displaySubObject(vao, shader);
	}
}

void myObject::displayObjects(myShader *shader, glm::mat4 view_matrix, const std::string& name)
{
	if (vao == nullptr)
	{
		std::cout << "myVAO is empty. Nothing to draw.\n";
		return;
	}

	shader->setUniform("mymodel_matrix", model_matrix);
	shader->setUniform("mynormal_matrix", normalMatrix(view_matrix));

	auto st = subObjects.equal_range(name);
	for (std::unordered_multimap<std::string, mySubObject *>::iterator it = st.first; it != st.second; ++it)
		it->second->displaySubObject(vao, shader);
}

void myObject::displayNormals(myShader *shader)
{
}

glm::vec3 myObject::closestVertex(glm::vec3 ray, glm::vec3 starting_point)
{
	float min = std::numeric_limits<float>::max();
	unsigned int min_index = 0;

	ray = glm::normalize(ray);
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		float dotp = glm::dot(ray, vertices[i] - starting_point);
		if (dotp < 0) continue;

		float oq = glm::distance(starting_point, vertices[i]);
		float d = oq*oq - dotp*dotp;
		if (d < min)
		{
			min = d;
			min_index = i;
		}
	}
	return vertices[min_index];
}


void myObject::translate(float x, float y, float z)
{
	glm::mat4 tmp = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	model_matrix = tmp * model_matrix;
}

void myObject::scale(float x, float y, float z)
{
	glm::mat4 tmp = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
	model_matrix = tmp * model_matrix;
}

void myObject::rotate(float axis_x, float axis_y, float axis_z, float angle)
{
	if (axis_x == 0.0f && axis_y == 0.0f && axis_z == 0.0f)
	{
		std::cout << "Error: rotation vector almost zero" << std::endl;
		return;
	}

	glm::mat4 tmp = glm::rotate(glm::mat4(1.0f), static_cast<float>(angle), glm::vec3(axis_x, axis_y, axis_z));
	model_matrix = tmp * model_matrix;
}

void myObject::translate(glm::vec3 v)
{
	translate(v.x, v.y, v.z);
}

void myObject::scale(glm::vec3 v)
{
	scale(v.x, v.y, v.z);
}

void myObject::rotate(glm::vec3 v, float angle)
{
	rotate(v.x, v.y, v.z, angle);
}

void myObject::computeTexturecoordinates_plane()
{
	texturecoords.assign(vertices.size(), glm::vec2(0.0f, 0.0f));
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		texturecoords[i].s = vertices[i].x / 10.0f;
		texturecoords[i].t = vertices[i].y / 10.0f;
	}
}

void myObject::computeTexturecoordinates_cylinder()
{
	texturecoords.assign(vertices.size(), glm::vec2(0.0f, 0.0f));
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		float x = vertices[i].x;
		float y = vertices[i].y;
		float z = vertices[i].z;

		texturecoords[i].t = y - 0.5f;
		texturecoords[i].s = static_cast<float>((z >= 0.0f) ? atan2(z, x) / (M_PI) : (-atan2(z, x)) / (M_PI));
	}
}

void myObject::computeTexturecoordinates_sphere()
{
	texturecoords.assign(vertices.size(), glm::vec2(0.0f, 0.0f));
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		glm::vec3 v = glm::normalize(vertices[i]);

		texturecoords[i].t = static_cast<float>(-(atan2(2 * v.y, 2 * v.x) + M_PI) / (2 * M_PI));
		texturecoords[i].s = static_cast<float>(acos(v.z) / M_PI);
	}
}

void myObject::computeTangents()
{
	tangents.assign(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		glm::vec2 t10 = texturecoords[indices[i][1]] - texturecoords[indices[i][0]];
		glm::vec2 t20 = texturecoords[indices[i][2]] - texturecoords[indices[i][0]];
		float f = t10.s*t20.t - t10.t*t20.s;
		if (f == 0) continue;
		f = 1.0f / f;
		glm::vec3 v10 = vertices[indices[i][1]] - vertices[indices[i][0]];
		glm::vec3 v20 = vertices[indices[i][2]] - vertices[indices[i][0]];
		glm::vec3 t(f*(t20.t*v10.x - t10.t*v20.x), f*(t20.t*v10.y - t10.t*v20.y),
			f*(t20.t*v10.z - t10.t*v20.z));

		tangents[indices[i][0]] += t;
		tangents[indices[i][1]] += t;
		tangents[indices[i][2]] += t;
	}
	for (unsigned int i = 0; i < vertices.size(); i++)  tangents[i] = glm::normalize(tangents[i]);
}


void myObject::setTexture(myTexture *tex, Texture_Type type)
{
	for (std::unordered_multimap<std::string, mySubObject *>::iterator it = subObjects.begin(); it != subObjects.end(); ++it)
		it->second->setTexture(tex, type);
}

void myObject::cleanTexture() {

	for (auto& sub : subObjects) {
		sub.second->textures.clear();
	}
}


float myObject::closestTriangle(glm::vec3 ray, glm::vec3 origin, size_t & picked_triangle)
{
	mySubObject *tmp;
	return closestTriangle(ray, origin, picked_triangle, tmp);
}

float myObject::closestTriangle(glm::vec3 ray, glm::vec3 origin, size_t & picked_triangle, mySubObject * &picked_object)
{
	float min_t = std::numeric_limits<float>::max();
	picked_triangle = 0;
	picked_object = nullptr;

	for (std::unordered_multimap<std::string, mySubObject *>::iterator it = subObjects.begin(); it != subObjects.end(); ++it)
	{
		mySubObject *obj = it->second;
		for (size_t i = obj->start; i < obj->end; i++)
		{
			glm::vec3 intersection_point;
			std::vector<glm::vec3> verts(3);
			for (unsigned int j : {0, 1, 2})
			{
				glm::vec4 t = model_matrix * glm::vec4(vertices[indices[i][j]], 1.0f);
				verts[j] = glm::vec3(t.x / t.w, t.y / t.w, t.z / t.w);
			}

			bool intersect = glm::intersectRayTriangle(origin, ray, verts[0], verts[1], verts[2], intersection_point);
			if (intersect)
			{
				float t = intersection_point.z;
				if (t >= 0 && t < min_t)
				{
					min_t = t;
					picked_triangle = i;
					picked_object = obj;
				}
			}
		}
	}

	if (picked_triangle == 0) return -1;
	else return min_t;
}