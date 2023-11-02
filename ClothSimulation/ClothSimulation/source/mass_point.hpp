#pragma once
struct Spring
{
	float restLength;
	float stiffness;
	int indexA;
	int indexB;

	Spring(float restLength, int indexA, int indexB, float stiffness)
		: restLength(restLength)
		, stiffness(stiffness)
		, indexA(indexA)
		, indexB(indexB)
	{}
};


struct MassPoint
{
	float mass;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	bool isSimulated;

	MassPoint(float mass, glm::vec3 position);
	MassPoint() = default;
	~MassPoint() = default;
};

