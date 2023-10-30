#include "mass_point.hpp"
#include "simulation_manager.hpp"


MassPoint::MassPoint(float mass, glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration)
{
	this->mass = mass;
	this->position = position;
	this->velocity = velocity;
	this->acceleration = acceleration;
	springs = std::vector<Spring>();
}

void MassPoint::setSprings(int i, int j, int m, int n)
{
	SimulationManager &simulationManager = SimulationManager::get();
	int massIndex = m * j + i;

	MassPoint &mA = simulationManager.get_mass_points()[massIndex];
	//flexion springs
	if (i - 2 >= 0) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex - 2];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex - 2));
	}
	if (i + 2 <= m) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex + 2];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex + 2));
	}
	if (j - 2 >= 0) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex - 2 * n];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex - 2 * n));
	}
	if (j + 2 <= n) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex + 2 * n];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex + 2 * n));
	}
	
	//shear springs
	if (i - 1 >= 0 && j - 1 >= 0) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex - n - 1];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex - n - 1));
	}
	if (i + 1 <= m && j - 1 >= 0) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex - n + 1];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex - n + 1));
	}
	if (i - 1 >= 0 && j + 1 <= n) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex + n - 1];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex + n - 1));
	}
	if (i + 1 <= m && j + 1 <= n) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex + n + 1];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex + n + 1));
	}

	//structural springs
	if (i - 1 >= 0) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex - 1];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex - 1));
	}
	if (i + 1 <= m) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex + 1];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex + 1));
	}
	if (j - 1 >= 0) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex - n];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex - n));
	}
	if (j + 1 <= n) {
		MassPoint &mB = simulationManager.get_mass_points()[massIndex + n];
		Float32 length = glm::length(mA.getPosition() - mB.getPosition());
		springs.push_back(Spring(length, massIndex, massIndex + n));
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
	return mass;
}

glm::vec3 MassPoint::getPosition()
{
	return position;
}

glm::vec3 MassPoint::getVelocity()
{
	return velocity;
}

glm::vec3 MassPoint::getAcceleration()
{
	return acceleration;
}

std::vector<Spring> MassPoint::getSprings()
{
	return springs;
}
