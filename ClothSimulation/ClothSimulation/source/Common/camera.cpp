#include "camera.hpp"

void Camera::initialize(const glm::vec3 &position)
{
	this->position = position;
	yaw = -90.0f;
	pitch = 0.0f;

	update_camera_vectors();
}

void Camera::move_forward(Float32 value)
{
	position += forward * value * speed;
}

void Camera::move_right(Float32 value)
{
	position += right * value * speed;
}

void Camera::move_up(Float32 value)
{
	position += glm::vec3(0.0f, 1.0f, 0.0f) * value * speed;
}

void Camera::rotate(Float32 xOffset, Float32 yOffset)
{
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch -= yOffset;

	if (pitch > 89.0f) 
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f) 
	{
		pitch = -89.0f;
	}

	update_camera_vectors();
}

const glm::mat4& Camera::get_view() const
{
	return glm::lookAt(position, position + forward, up);
}

const glm::mat4& Camera::get_projection(const Float32 aspectRatio) const
{
	return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::update_camera_vectors()
{
	forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward.y = sin(glm::radians(pitch));
	forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(forward);

	right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, forward));
}
