#include "LightList.h"

LightList::LightList()
{
}


LightList::~LightList()
{
}

void LightList::addLight(std::unique_ptr<Light> light)
{
	list.push_back(std::move(light));
}
