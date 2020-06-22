#pragma once

class Camera
{
public:
	glm::vec3 camera_up;
	glm::vec3 camera_eye;
	glm::vec3 camera_forward;

	float fovY;
	float farZ;
	float nearZ;
	
	//Window parameters
	int window_width;
	int window_height;

	Camera();
	~Camera();
	void reset();

	void crystalball_rotateView(int dx, int dy);
	void firstperson_rotateView(int dx, int dy);
	void panView(int dx, int dy);

	void moveForward(float size);
	void moveBack(float size);
	void turnLeft(float size);
	void turnRight(float size);

	void print() const;

	glm::vec3 constructRay(int x, int y) const;
	
	glm::mat4 projectionMatrix() const;
	glm::mat4 viewMatrix() const;
	glm::mat4 weivMatrix() const;

private:

	mutable bool project_outdated = true;
	mutable bool view_outdated = true;
	mutable bool weiv_outdated = true;

	mutable glm::mat4 project_matrix;
	mutable glm::mat4 view_matrix;
	mutable glm::mat4 weiv_matrix;

	inline void outdate();
};
