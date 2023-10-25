#pragma once
class Camera
{
public:
	glm::vec3 forward, up, right;
	glm::vec3 position;
	glm::quat rotation;
};

