#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#define GLM_FORCE_AVX
#include <glm/glm.hpp>

static std::vector<glm::vec4> kernelSSSS = {
	glm::vec4(0.530605, 0.613514, 0.739601, 0),
	glm::vec4(0.000973794, 1.11862e-005, 9.43437e-007, -3),
	glm::vec4(0.00333804, 7.85443e-005, 1.2945e-005, -2.52083),
	glm::vec4(0.00500364, 0.00020094, 5.28848e-005, -2.08333),
	glm::vec4(0.00700976, 0.00049366, 0.000151938, -1.6875),
	glm::vec4(0.0094389, 0.00139119, 0.000416598, -1.33333),
	glm::vec4(0.0128496, 0.00356329, 0.00132016, -1.02083),
	glm::vec4(0.017924, 0.00711691, 0.00347194, -0.75),
	glm::vec4(0.0263642, 0.0119715, 0.00684598, -0.520833),
	glm::vec4(0.0410172, 0.0199899, 0.0118481, -0.333333),
	glm::vec4(0.0493588, 0.0367726, 0.0219485, -0.1875),
	glm::vec4(0.0402784, 0.0657244, 0.04631, -0.0833333),
	glm::vec4(0.0211412, 0.0459286, 0.0378196, -0.0208333),
	glm::vec4(0.0211412, 0.0459286, 0.0378196, 0.0208333),
	glm::vec4(0.0402784, 0.0657244, 0.04631, 0.0833333),
	glm::vec4(0.0493588, 0.0367726, 0.0219485, 0.1875),
	glm::vec4(0.0410172, 0.0199899, 0.0118481, 0.333333),
	glm::vec4(0.0263642, 0.0119715, 0.00684598, 0.520833),
	glm::vec4(0.017924, 0.00711691, 0.00347194, 0.75),
	glm::vec4(0.0128496, 0.00356329, 0.00132016, 1.02083),
	glm::vec4(0.0094389, 0.00139119, 0.000416598, 1.33333),
	glm::vec4(0.00700976, 0.00049366, 0.000151938, 1.6875),
	glm::vec4(0.00500364, 0.00020094, 5.28848e-005, 2.08333),
	glm::vec4(0.00333804, 7.85443e-005, 1.2945e-005, 2.52083),
	glm::vec4(0.000973794, 1.11862e-005, 9.43437e-007, 3)
};

class SSSS {

public:
	float width;
	glm::vec3 falloff;
	glm::vec3 strength;
	SSSS(int sampleNumber);

	void setSampleCount(int count);
	std::vector<glm::vec4> kernel;

private:
	size_t sampleCount;
	std::string getKernelCode() const;

	void calculateKernel();
	glm::vec3 profile(float r);
	glm::vec3 gaussian(float variance, float r);
};