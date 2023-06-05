#include "MeshPack.h"
#include "helperFunctions.h"

#include <glm/gtx/hash.hpp>
#include <glm/gtx/intersect.hpp>

MeshPack::MeshPack()
{
	model_matrix = glm::mat4(1.0f);
	name = "default";
	vao = nullptr;
}

MeshPack::~MeshPack()
{
	clear();
}

void MeshPack::clear()
{
	for (auto it = children.begin(); it != children.end(); ++it) {
		delete it->second;
	}

	children.clear();

	std::vector<glm::vec3> empty_f; 
	vertices.swap(empty_f);
	v_normals.swap(empty_f);

	std::vector<glm::ivec3> empty_i; 
	indices.swap(empty_i);
}

bool MeshPack::readObjects(const std::string& filename, bool allow_duplication, bool tonormalize, bool subgroup)
{
	clear();

	std::string s, t;
	std::string mtlfilename;

	std::ifstream fin(filename);
	if (!fin.is_open()) return false;

	size_t curr_start = 0, curr_end;
	std::string curr_name = "noname";
	Material *curr_mat = nullptr;
	Texture *curr_texture = nullptr;

	std::unordered_map<std::string, Material *> materials;
	//std::unordered_map<std::string, myTexture *> textures;

	std::vector<glm::vec3> tmp_vertices;
	std::vector<glm::vec3> tmp_normals;
	std::vector<glm::vec2> tmp_coords;

	std::unordered_map<glm::vec3, size_t> cached;

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
			MeshPart *o = new MeshPart(curr_mat, curr_start, curr_end, curr_name);
			children.emplace(curr_name, o);

			curr_start = curr_end;
			myline >> curr_name;
		}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			tmp_vertices.emplace_back(glm::vec3(x, y, z));
		}
		else if (t == "vn")
		{
			float x, y, z;
			myline >> x >> y >> z;
			tmp_normals.emplace_back(glm::vec3(x, y, z));
		}
		else if (t == "vt")
		{
			float _s, _t;
			myline >> _s >> _t;
			tmp_coords.emplace_back(glm::vec2(_s, 1.0f - _t));
		}
		else if (t == "mtllib")
		{
			myline >> mtlfilename;
			size_t prefix = filename.find_last_of("/");
			//if (prefix != std::string::npos)
				//mtlfilename = (filename.substr(0, prefix)).c_str() + std::string("/") + mtlfilename;
			//readMaterials(mtlfilename, materials, textures);
		}
		else if (t == "usemtl")
		{
			curr_end = indices.size();
			//MeshPart *o = new MeshPart(curr_mat, curr_start, curr_end, curr_name);
			//o->setTexture(curr_texture, Texture_Type::colortex);
			//children.emplace(curr_name, o);

			curr_start = curr_end;

			std::string u;
			myline >> u;
			curr_name = u;

			//if (materials.count(u) != 0)
			//	curr_mat = materials[u];
			//else curr_mat = nullptr;

			//if (textures.count(u) != 0)
			//	curr_texture = textures[u];
			//else curr_texture = nullptr;
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
					auto transfer = [&](uint32_t v_idx, uint32_t t_idx, uint32_t n_idx) -> size_t {

						auto index = vertices.size();
						vertices.push_back(tmp_vertices[v_idx]);

						if (n_idx < tmp_normals.size())
							v_normals.push_back(tmp_normals[n_idx]);
						else v_normals.emplace_back(glm::vec3(0, 0, 0));

						if (t_idx < tmp_coords.size())
							texcoords.push_back(tmp_coords[t_idx]);
						else texcoords.emplace_back(glm::vec2(0, 0));

						return index;
					};

					auto cached_a = transfer(vertex_idx_a, texture_idx_a, normal_idx_a);
					auto cached_b = transfer(vertex_idx_b, texture_idx_b, normal_idx_b);
					auto cached_c = transfer(vertex_idx_c, texture_idx_c, normal_idx_c);

					indices.emplace_back(glm::ivec3(cached_a, cached_b, cached_c));
				}
				else {

					auto cacheCheck = [&](uint32_t v_idx, uint32_t t_idx, uint32_t n_idx) -> size_t { // ignore normal

						auto tmp_v = tmp_vertices[v_idx];

						if (cached.count(tmp_v) == 0) {
							cached[tmp_v] = vertices.size();
							auto result = vertices.size();
							vertices.push_back(tmp_v);

							if (tmp_normals.size() > 0 && n_idx < tmp_normals.size()) {
								auto tmp_n = tmp_normals[n_idx];
								v_normals.push_back(tmp_n);
							}

							if (tmp_coords.size() > 0 && t_idx < tmp_coords.size()) {
								auto tmp_t = tmp_coords[t_idx];
								texcoords.push_back(tmp_t);
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

					indices.emplace_back(glm::ivec3(cached_a, cached_b, cached_c));
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

		MeshPart* o = new MeshPart(curr_mat, curr_start, curr_end, curr_name);
		children.emplace(curr_name, o);
	}
	else {

		MeshPart* o = new MeshPart(curr_mat, 0, indices.size(), curr_name);
		children.clear(); children.emplace(curr_name, o);
	}

	if (tmp_normals.size() == 0) {
		computeNormals();
	}

	if (tonormalize) normalize();

	return true;
}

void MeshPack::normalize()
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

void MeshPack::computeNormals()
{
	f_normals.assign(indices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
	v_normals.assign(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		auto a = vertices[indices[i][1]] - vertices[indices[i][0]];
		auto b = vertices[indices[i][2]] - vertices[indices[i][1]];

		glm::vec3 face_normal = glm::cross(a, b);
		f_normals[i] = face_normal;

		v_normals[indices[i][0]] += face_normal;  
		v_normals[indices[i][1]] += face_normal;  
		v_normals[indices[i][2]] += face_normal;  
	}

	std::unordered_map<glm::vec3, glm::vec3> cached; 

	for (unsigned int i = 0; i < vertices.size(); i++) {
		auto& v = vertices[i];
		auto& n = v_normals[i];

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

		v_normals[i] = glm::normalize(n);
	}
}

void MeshPack::createVAO()
{
	vao = std::make_unique<VAO>();

	if (indices.size()) vao->storeIndices(indices);

	if (vertices.size()) vao->storePositions(vertices, 0);
	if (v_normals.size()) vao->storeNormals(v_normals, 1);
	if (texcoords.size()) vao->storeTexcoords(texcoords, 2);
}

glm::mat4 MeshPack::normalMatrix(glm::mat4& view_matrix, glm::mat4& instance_matrix) {
	return glm::transpose(glm::inverse(view_matrix * instance_matrix));
}

void MeshPack::batch() {
	modelMatrixList.push_back(model_matrix);
}

void MeshPack::displayObjects(std::unique_ptr<Shader> const& shader, glm::mat4& view_matrix)
{
	myassert(vao != nullptr);

	if (modelMatrixList.size() == 0) {
		shader->setUniform("model_matrix", model_matrix);
		auto matrix = normalMatrix(view_matrix, model_matrix);
		shader->setUniform("normal_matrix", matrix);
		//batch();
	}

	std::vector<glm::mat4> normalMatrixList;
	for (auto& matrix : modelMatrixList) {
		normalMatrixList.emplace_back(normalMatrix(view_matrix, matrix));
	}

	shader->setUniform("view_matrix", view_matrix);

	for (auto it = children.begin(); it != children.end(); ++it) {

		if (modelMatrixList.size() > 0) {
			it->second->display(vao, shader, modelMatrixList, normalMatrixList);
		} else {
			it->second->display(vao, shader);
		}
	}

	modelMatrixList.clear();
}

void MeshPack::translate(glm::vec3 v)
{
	glm::mat4 tmp = glm::translate(glm::mat4(1.0f), v);
	model_matrix = tmp * model_matrix;
}

void MeshPack::translate(float x, float y, float z)
{
	translate(glm::vec3(x, y, z));
}

void MeshPack::scale(glm::vec3 v)
{
	glm::mat4 tmp = glm::scale(glm::mat4(1.0f), v);
	model_matrix = tmp * model_matrix;
}

void MeshPack::scale(float x, float y, float z)
{
	if (x == 0 || y == 0 || z == 0) 
	{
		std::cout << "Error: scale is almost zero" << std::endl;
		return;
	}

	scale(glm::vec3(x, y, z));
}

void MeshPack::rotate(glm::vec3 axis, float angle) {

	glm::mat4 tmp = glm::rotate(glm::mat4(1.0f), static_cast<float>(angle), axis);
	model_matrix = tmp * model_matrix;
}

void MeshPack::rotate(float axis_x, float axis_y, float axis_z, float angle)
{
	if (axis_x == 0.0f && axis_y == 0.0f && axis_z == 0.0f)
	{
		std::cout << "Error: rotation vector almost zero" << std::endl;
		return;
	}

	rotate(glm::vec3(axis_x, axis_y, axis_z), angle);
}

void MeshPack::computeTexturecoordinates_plane()
{
	texcoords.assign(vertices.size(), glm::vec2(0.0f, 0.0f));
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		texcoords[i].s = vertices[i].x / 10.0f;
		texcoords[i].t = vertices[i].y / 10.0f;
	}
}

void MeshPack::computeTexturecoordinates_cylinder()
{
	texcoords.assign(vertices.size(), glm::vec2(0.0f, 0.0f));
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		float x = vertices[i].x;
		float y = vertices[i].y;
		float z = vertices[i].z;

		texcoords[i].t = y - 0.5f;
		texcoords[i].s = static_cast<float>((z >= 0.0f) ? atan2(z, x) / (M_PI) : (-atan2(z, x)) / (M_PI));
	}
}

void MeshPack::computeTexturecoordinates_sphere()
{
	texcoords.assign(vertices.size(), glm::vec2(0.0f, 0.0f));
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		glm::vec3 v = glm::normalize(vertices[i]);

		texcoords[i].t = static_cast<float>(-(atan2(2 * v.y, 2 * v.x) + M_PI) / (2 * M_PI));
		texcoords[i].s = static_cast<float>(acos(v.z) / M_PI);
	}
}

void MeshPack::computeTangents()
{
	tangents.assign(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		glm::vec2 t10 = texcoords[indices[i][1]] - texcoords[indices[i][0]];
		glm::vec2 t20 = texcoords[indices[i][2]] - texcoords[indices[i][0]];
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

void MeshPack::setTexture(Texture* tex, Texture_Type type)
{
	for (auto& sub : children) {
		sub.second->setTexture(tex, type);
	}
}

void MeshPack::clearTexture() {

	for (auto& sub : children) {
		sub.second->textures.clear();
	}
}

glm::vec3 MeshPack::closestVertex(glm::vec3 ray, glm::vec3 starting_point)
{
	float min = std::numeric_limits<float>::max();
	unsigned int min_index = 0;

	ray = glm::normalize(ray);
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		float dotp = glm::dot(ray, vertices[i] - starting_point);
		if (dotp < 0) continue;

		float oq = glm::distance(starting_point, vertices[i]);
		float d = oq * oq - dotp * dotp;
		if (d < min)
		{
			min = d;
			min_index = i;
		}
	}
	return vertices[min_index];
}

float MeshPack::closestTriangle(glm::vec3 ray, glm::vec3 origin, size_t & picked_triangle)
{
	MeshPart *tmp;
	return closestTriangle(ray, origin, picked_triangle, tmp);
}

float MeshPack::closestTriangle(glm::vec3 ray, glm::vec3 origin, size_t & picked_triangle, MeshPart * &picked_object)
{
	float min_t = std::numeric_limits<float>::max();
	picked_triangle = 0;
	picked_object = nullptr;

	for (auto it = children.begin(); it != children.end(); ++it)
	{
		MeshPart *obj = it->second;
		for (size_t i = obj->start; i < obj->end; i++)
		{
			glm::vec3 intersection_point;
			std::vector<glm::vec3> verts(3);
			for (unsigned int j : {0, 1, 2})
			{
				glm::vec4 t = model_matrix * glm::vec4(vertices[indices[i][j]], 1.0f);
				verts[j] = glm::vec3(t.x / t.w, t.y / t.w, t.z / t.w);
			}

			// bool intersect = glm::intersectRayTriangle(origin, ray, verts[0], verts[1], verts[2], intersection_point);
			// if (intersect)
			// {
			// 	float t = intersection_point.z;
			// 	if (t >= 0 && t < min_t)
			// 	{
			// 		min_t = t;
			// 		picked_triangle = i;
			// 		picked_object = obj;
			// 	}
			// }
		}
	}

	if (picked_triangle == 0) return -1;
	else return min_t;
}