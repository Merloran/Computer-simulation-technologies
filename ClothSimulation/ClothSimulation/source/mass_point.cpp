#include "pch.hpp"
#include "mass_point.hpp"


MassPoint::MassPoint(float mass, glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration)
{
	this->mass = mass;
	this->position = position;
	this->velocity = velocity;
	this->acceleration = acceleration;
	springs = std::vector<Spring>();
}

MassPoint::~MassPoint()
{
}

void MassPoint::setSprings(int i, int j, int m, int n, int** grid)
{
	int massIndex = m * j + i;

	//flexion springs
	if (i - 2 >= 0) {
		springs.push_back(Spring(2.f,massIndex, massIndex - 2));
	}
	if (i + 2 <= m) {
		springs.push_back(Spring(2.f, massIndex, massIndex + 2));
	}
	if (j - 2 >= 0) {
		springs.push_back(Spring(2.f, massIndex, massIndex - 2 * j));
	}
	if (j + 2 <= n) {
		springs.push_back(Spring(2.f, massIndex, massIndex + 2 * j));
	}
	
	//shear springs
	if (i - 1 >= 0 && j - 1 >= 0) {
		springs.push_back(Spring(1.414f, massIndex, massIndex - j - 1));
	}
	if (i + 1 <= m && j - 1 >= 0) {
		springs.push_back(Spring(1.414f, massIndex, massIndex - j + 1));
	}
	if (i - 1 >= 0 && j + 1 <= n) {
		springs.push_back(Spring(1.414f, massIndex, massIndex + j - 1));
	}
	if (i + 1 <= m && j + 1 <= n) {
		springs.push_back(Spring(1.414f, massIndex, massIndex + j + 1));
	}

	//structural springs
	if (i - 1 >= 0) {
		springs.push_back(Spring(1.f, massIndex, massIndex - 1));
	}
	if (i + 1 <= m) {
		springs.push_back(Spring(1.f, massIndex, massIndex + 1));
	}
	if (j - 1 >= 0) {
		springs.push_back(Spring(1.f, massIndex, massIndex - j));
	}
	if (j + 1 <= n) {
		springs.push_back(Spring(1.f, massIndex, massIndex + j));
	}
}

void MassPoint::setMass(float mass)
{
 this->mass = mass;
}

void MassPoint::setPosition(glm::vec3 position)
{
  this->position = position;
}

void MassPoint::setVelocity(glm::vec3 velocity)
{
  this->velocity = velocity;
}

void MassPoint::setAcceleration(glm::vec3 acceleration)
{
	this->acceleration = acceleration;
}

float MassPoint::getMass()
{
	return 0.0f;
}

glm::vec3 MassPoint::getPosition()
{
	return glm::vec3();
}

glm::vec3 MassPoint::getVelocity()
{
	return glm::vec3();
}

glm::vec3 MassPoint::getAcceleration()
{
	return glm::vec3();
}

std::vector<Spring> MassPoint::getSprings()
{
	return std::vector<Spring>();
}
