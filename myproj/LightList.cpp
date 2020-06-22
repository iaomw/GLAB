#include "LightList.h"

LightList::LightList()
{
}


LightList::~LightList()
{
}

void LightList::addLight(Light& light)
{
	auto shadowFBO = std::make_unique<CubeFBO>(true);

	shadowFBO->initFBO(1024, 1024);
	light.shadow_handle = shadowFBO->envTexture->texture_handle;

	list.push_back(std::move(light));
	shadowList.push_back(std::move(shadowFBO));
}
