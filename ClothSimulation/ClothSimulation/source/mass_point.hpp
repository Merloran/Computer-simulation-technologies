#pragma once
struct Spring
{
	float restLength;
	int indexA;
	int indexB;
	Spring(float restLength, int indexA, int indexB) {
		this->restLength = restLength;
		this->indexA = indexA;
		this->indexB = indexB;
	}
};


class MassPoint
{
private:
	float mass;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	std::vector<Spring> springs;

public:
	MassPoint(float mass, glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration);
	MassPoint() = default;
	~MassPoint() = default;


	void setSprings(int i, int j, int m, int n);

	void setMass(float mass);
	void setPosition(glm::vec3 position);
	void setVelocity(glm::vec3 velocity);
	void setAcceleration(glm::vec3 acceleration);

	float getMass();
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	glm::vec3 getAcceleration();

	std::vector<Spring> getSprings();
};

