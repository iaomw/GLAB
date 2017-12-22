#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "myVAO.h"
#include "myMaterial.h"
#include "myTexture.h"
#include <map>

class myShader;

class mySubObject
{
public:
	size_t start, end;
	myMaterial *material;

	enum TEXTURE_TYPE { COLORMAP, 
						BUMPMAP, 
						CUBEMAP,

						pAlbedo,
						pAO,
						pHeight,
						pMetal,
						pNormal,
						pRough,

						gPosition,
						gAlbedo,
						gNormal,
						gEnv,
						
						Unknow };

	std::map<TEXTURE_TYPE, myTexture *> textures;

	std::string name;

	mySubObject(myMaterial *m, size_t s, size_t e, std::string n) ;
	mySubObject(size_t s, size_t e, std::string);
	
	~mySubObject();

	void setTexture(myTexture *t, TEXTURE_TYPE);

	void displaySubObject(myVAO *vao, myShader *);
	void displayNormals(myVAO *vao, myShader *);
};
