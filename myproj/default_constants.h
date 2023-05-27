#pragma once
#include "glm/glm.hpp"

const glm::vec3 DEFAULT_CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 DEFAULT_CAMERA_EYE = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 DEFAULT_CAMERA_FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);

const float DEFAULT_FOVY = 45.0f;
const float DEFAULT_zNEAR = 1.0f;
const float DEFAULT_zFAR = 2000.0f;

const int DEFAULT_WINDOW_HEIGHT = 900;
const int DEFAULT_WINDOW_WIDTH = 1600;

const glm::vec4 DEFAULT_KD = glm::vec4(0, 1, 0, 0);
const glm::vec4 DEFAULT_KS = glm::vec4(0, 0, 0, 0);
const glm::vec4 DEFAULT_KA = glm::vec4(0.1, 0.1, 0.1, 0);
const float DEFAULT_SPECULAR_COEFFICIENT = 20.0f;

const float DEFAULT_KEY_MOVEMENT_STEPSIZE = 1.0f;
const float DEFAULT_MOUSEWHEEL_MOVEMENT_STEPSIZE = 0.5f;
const float DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE = 0.01f;

const glm::vec3 DEFAULT_LIGHT_POSITION = glm::vec3(0, 0, 0);
const glm::vec3 DEFAULT_LIGHT_INTENSITY = glm::vec3(1, 1, 1);
const glm::vec3 DEFAULT_LIGHT_DIRECTION = glm::vec3(0, 0, -1);