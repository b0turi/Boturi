#pragma once

class Camera
{
private:
	float fieldOfView;
	glm::mat4 projectionMatrix;
public:
	glm::vec3 position;
	glm::vec3 rotation;

	Camera(float fov);

	void rotate(float xAmt, float yAmt);

	glm::mat4 viewMatrix();
	glm::mat4 setProjectionMatrix();
	glm::mat4 getProjectionMatrix();
};