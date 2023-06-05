#pragma once
#include "FBO.h"

class GeoFBO: public FBO
{
public:

	std::unique_ptr<Texture> texAlbedo;
	std::unique_ptr<Texture> texNormal;

	GeoFBO();
	~GeoFBO();

	virtual void reset();

	void multiDraw(std::unique_ptr<Shader> const& shader, std::unique_ptr<MeshPack> &object, glm::mat4 view_matrix, std::vector<glm::vec3>& pos_list) {

		this->bind();
		shader->start();
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		object->modelMatrixList.clear();
		
		for (auto& trans : pos_list) {

			object->translate(trans.x, trans.y, trans.z);
			object->batch();
			object->translate(-trans.x, -trans.y, -trans.z);
		};

		object->displayObjects(shader, view_matrix);

		shader->stop();
		this->unbind();
	}

	void initFBO(int WIDTH, int HEIGHT);
};

