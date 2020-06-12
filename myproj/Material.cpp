#include "Material.h"
#include "default_constants.h"

Material::Material()
{
	kd = DEFAULT_KD;
	ks = DEFAULT_KS;
	ka = DEFAULT_KA;
	specular_coefficient = DEFAULT_SPECULAR_COEFFICIENT;
}

Material::Material(glm::vec4 m_kd, glm::vec4 m_ks, glm::vec4 m_ka, float specular)
	:kd(m_kd), ks(m_ks), ka(m_ka), specular_coefficient(specular)
{
}

Material::Material(Material *m)
	:Material(m->kd, m->ks, m->ka, m->specular_coefficient)
{}


Material::~Material()
{
}

void Material::setUniform(std::unique_ptr<Shader> const& shader, const std::string& name)
{
	shader->setUniform(name + ".kd", kd);
	shader->setUniform(name + ".ks", ks);
	shader->setUniform(name + ".ka", ka);
	shader->setUniform(name + ".specular", specular_coefficient);
}
