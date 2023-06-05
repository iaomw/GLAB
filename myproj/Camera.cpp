#include "Camera.h"
#include "helperFunctions.h"
#include "default_constants.h"

Camera::Camera()
{
	camera_up = DEFAULT_CAMERA_UP;
	camera_eye = DEFAULT_CAMERA_EYE;
	camera_forward = DEFAULT_CAMERA_FORWARD;

	fovY = DEFAULT_FOVY;
	farZ = DEFAULT_zFAR;
	nearZ = DEFAULT_zNEAR;
	
	window_width = DEFAULT_WINDOW_WIDTH;
	window_height = DEFAULT_WINDOW_HEIGHT;
}

Camera::~Camera()
{
}

void Camera::reset()
{
	camera_up = DEFAULT_CAMERA_UP;
	camera_eye = DEFAULT_CAMERA_EYE;
	camera_forward = DEFAULT_CAMERA_FORWARD;

	fovY = DEFAULT_FOVY;
	farZ = DEFAULT_zFAR;
	nearZ = DEFAULT_zNEAR;
}

void Camera::crystalball_rotateView(int dx, int dy)
{
	if (dx == 0 && dy == 0) return;

	float vx = static_cast<float>(dx) / static_cast<float>(window_width);
	float vy = -static_cast<float>(dy) / static_cast<float>(window_height);

	float theta = 4.0f * (fabs(vx) + fabs(vy));

	glm::vec3 camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
	glm::vec3 tomovein_direction = -camera_right * vx + -camera_up * vy;

	glm::vec3 rotation_axis = glm::normalize(glm::cross(tomovein_direction, camera_forward));

	rotate(camera_forward, rotation_axis, theta, true);
	rotate(camera_up, rotation_axis, theta, true);
	rotate(camera_eye, rotation_axis, theta, false);

	outdate();
}

void Camera::firstperson_rotateView(int dx, int dy)
{
	if (dx == 0 && dy == 0) return;

	float vx = static_cast<float>(dx) / static_cast<float>(window_width);
	float vy = -static_cast<float>(dy) / static_cast<float>(window_height);

	float theta = 4.0f * (fabs(vx) + fabs(vy));

	glm::vec3 camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
	glm::vec3 tomovein_direction = -camera_right * vx + -camera_up * vy;

	glm::vec3 rotation_axis = glm::normalize(glm::cross(tomovein_direction, camera_forward));

	rotate(camera_forward, rotation_axis, theta, true);
	//rotate(camera_up, rotation_axis, theta, true);

	outdate();
}

void Camera::panView(int dx, int dy)
{
	if (dx == 0 && dy == 0) return;

	float vx = static_cast<float>(dx) / static_cast<float>(window_width);
	float vy = -static_cast<float>(dy) / static_cast<float>(window_height);

	glm::vec3 camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
	glm::vec3 tomovein_direction = -camera_right * vx + -camera_up * vy;

	camera_eye += 1.6f * tomovein_direction;

	outdate();
}

glm::vec3 Camera::constructRay(int x, int y) const
{
	glm::mat4 projection_matrix = projectionMatrix( );
	glm::mat4 view_matrix = viewMatrix();

	y = window_height - y;

	float x_c = (2.0f*x) / static_cast<float>(window_width) - 1.0f;
	float y_c = (2.0f*y) / static_cast<float>(window_height) - 1.0f;

	glm::vec4 tmp = glm::vec4(x_c, y_c, -1.0f, 1.0f);
	tmp = glm::inverse(projection_matrix) * tmp;
	tmp.z = -1.0f; tmp.w = 0.0f;

	return glm::normalize(glm::vec3(glm::inverse(view_matrix) * tmp));
}

glm::mat4 Camera::projectionMatrix() const
{
	if (project_outdated) {
		project_matrix = glm::perspective(glm::radians(fovY), static_cast<float>(window_width) / static_cast<float>(window_height), nearZ, farZ);
		project_outdated = false;
	}
	return project_matrix;
}

glm::mat4 Camera::viewMatrix() const
{
	if (view_outdated) {
		view_matrix = glm::lookAt(camera_eye, camera_eye + camera_forward, camera_up);
		view_outdated = false;
	}
	return view_matrix;
}

glm::mat4 Camera::weivMatrix() const
{
	if (weiv_outdated) {
		weiv_matrix = glm::inverse(viewMatrix());
		weiv_outdated = false;
	}
	return weiv_matrix;
}

void Camera::moveForward(float size)
{
	camera_eye += size * camera_forward;
	outdate();
}

void Camera::moveBack(float size)
{
	camera_eye -= size * camera_forward;
	outdate();
}

void Camera::turnLeft(float size)
{
	rotate(camera_forward, camera_up, size, true);
	outdate();
}

void Camera::turnRight(float size)
{
	rotate(camera_forward, camera_up, -size, true);
	outdate();
}

void Camera::print() const
{
	std::cout << "Eye: (" << camera_eye.x << ", " << camera_eye.y << ", " << camera_eye.z << ")" << std::endl;
	std::cout << "Forward: (" << camera_forward.x << ", " << camera_forward.y << ", " << camera_forward.z << ")" << std::endl;
	std::cout << "Up: (" << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << ")" << std::endl;
}

inline void Camera::outdate() {
	project_outdated = true;
	view_outdated = true;
	weiv_outdated = true;
}
