#include "Boturi.h"

Camera::Camera(float fov)
{
	position = glm::vec3(0, 3, 3);
	rotation = glm::vec3(0, 0, 0);

	fieldOfView = fov;
}

glm::mat4 Camera::viewMatrix()
{
	glm::mat4 view = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0)) *
		glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0)) *
		glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1)) *
		glm::translate(glm::mat4(1.0f), -position);

	return view;

}

glm::mat4 Camera::setProjectionMatrix()
{
	projectionMatrix = glm::perspective(glm::radians(fieldOfView), Boturi::aspectRatio, 0.1f, 10.0f);
	projectionMatrix[1][1] *= -1;

	return projectionMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::rotate(float xAmt, float yAmt)
{
	rotation.y += xAmt/ Boturi::extent.width;
	rotation.x += yAmt/ Boturi::extent.height;
}