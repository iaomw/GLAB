#include "SSSS.h"

//SSSS::SSSS {};
//SSSS::~SSSS{};

SSSS::SSSS(int sampleNumber)
{
	sampleCount = sampleNumber;
	falloff = glm::vec3(0.4f);
	strength = glm::vec4(0.6f);

	falloff = glm::vec3(1.0f, 0.37f, 0.3f);
	strength = glm::vec3(0.48f, 0.41f, 0.28f);

	calculateKernel();
}

void SSSS::setSampleCount(int count) {
	if (count == sampleCount) return;
	sampleCount = count;
	calculateKernel();
}

glm::vec3 SSSS::gaussian(float variance, float r) {
	/**
	 * We use a falloff to modulate the shape of the profile. Big falloffs
	 * spreads the shape making it wider, while small falloffs make it
	 * narrower.
	 */
	glm::vec3 g;
	for (int i = 0; i < 3; i++) {
		float rr = r / (0.001f + falloff[i]);
		g[i] = exp((-(rr * rr)) / (2.0f * variance)) / (2.0f * 3.14f * variance);
	}
	return g;
}

glm::vec3 SSSS::profile(float r) {
	/**
	*We used the red channel of the original skin profile defined in
	* [d'Eon07] for all three channels. We noticed it can be used for green
	* andblue channels(scaled using the falloff parameter) without
	* introducing noticeable differences and allowing for total control over
	* the profile.For example, it allows to create blue SSS gradients, which
	* could be useful in case of rendering blue creatures.
	*/
	return  //0.233f * gaussian(0.0064f, r) + /* We consider this one to be directly bounced light, accounted by the strength parameter (see @STRENGTH) */
	0.100f * gaussian(0.0484f, r) +
	0.118f * gaussian(0.187f, r) +
	0.113f * gaussian(0.567f, r) +
	0.358f * gaussian(1.99f, r) +
	0.078f * gaussian(7.41f, r);
}

void SSSS::calculateKernel() {

	const float RANGE = sampleCount > 20 ? 3.0f : 2.0f;
	const float EXPONENT = 2.0f;

	kernel.resize(sampleCount);

	// Calculate the offsets:
	float step = 2.0f * RANGE / (sampleCount - 1);
	for (int i = 0; i < sampleCount; i++) {
		float o = -RANGE + float(i) * step;
		float sign = o < 0.0f ? -1.0f : 1.0f;
		kernel[i].w = RANGE * sign * abs(pow(o, EXPONENT)) / pow(RANGE, EXPONENT);
	}

	// Calculate the weights:
	for (size_t i = 0; i < sampleCount; i++) {
		float w0 = i > 0 ? abs(kernel[i].w - kernel[i - (size_t)1].w) : 0.0f;
		float w1 = i < (sampleCount - (size_t)1) ? abs(kernel[i].w - kernel[i + (size_t)1].w) : 0.0f;
		float area = (w0 + w1) / 2.0f;
		glm::vec3 t = area * profile(kernel[i].w);
		kernel[i].x = t.x;
		kernel[i].y = t.y;
		kernel[i].z = t.z;
	}

	// We want the offset 0.0 to come first:
	glm::vec4 t = kernel[sampleCount / 2];
	for (size_t i = sampleCount / 2; i > 0; i--)
		kernel[i] = kernel[i - (size_t)1];
	kernel[0] = t;

	// Calculate the sum of the weights, we will need to normalize them below:
	glm::vec3 sum = glm::vec3(0.0f);
	for (int i = 0; i < sampleCount; i++) {
		sum += glm::vec3(kernel[i]);
	}
	// Normalize the weights:
	for (int i = 0; i < sampleCount; i++) {
		kernel[i].x /= sum.x;
		kernel[i].y /= sum.y;
		kernel[i].z /= sum.z;
	}

	// Tweak them using the desired strength. The first one is:
	//     lerp(1.0, kernel[0].rgb, strength)
	kernel[0].x = (1.0f - strength.x) * 1.0f + strength.x * kernel[0].x;
	kernel[0].y = (1.0f - strength.y) * 1.0f + strength.y * kernel[0].y;
	kernel[0].z = (1.0f - strength.z) * 1.0f + strength.z * kernel[0].z;

	// The others:
	//     lerp(0.0, kernel[0].rgb, strength)
	for (int i = 1; i < sampleCount; i++) {
		kernel[i].x *= strength.x;
		kernel[i].y *= strength.y;
		kernel[i].z *= strength.z;
	}
}

std::string SSSS::getKernelCode() const {
	std::stringstream s;
	s << "float4 kernel[] = {" << "\r\n";
	for (int i = 0; i < sampleCount; i++)
		s << "    float4(" << kernel[i].x << ", "
		<< kernel[i].y << ", "
		<< kernel[i].z << ", "
		<< kernel[i].w << ")," << "\r\n";
	s << "};" << "\r\n";
	return s.str();
}
