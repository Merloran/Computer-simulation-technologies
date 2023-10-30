#include "pch.hpp"
#include "simulation_manager.hpp"
#include <glm/gtx/norm.hpp>

void SimulationManager::startup()
{
}

void SimulationManager::update()
{
	
	for (int i = 0; i < numberOfMasses; i++)
	{
		positions[i] = massPoints[i].getPosition();
		velocities[i] = massPoints[i].getVelocity();
		accelerations[i] = massPoints[i].getAcceleration();
	}

	for (int i = 0; i < numberOfMasses; i++)
	{
		internalForces[i] = getInternalForce(i);
		externalForces[i] = getExternalForce(i);
	}

	int iter = 0;
	float micro = 1.f;
	float tiemstep = 0.2f;

	float variation = 0.f;

	while (variation > variationThreshold || (iter < minIterations)) {

		variation = 0.f;
		glm::vec3 varVector = glm::vec3(0.0f);

		for (int i = 0; i < numberOfMasses; i++)
		{
			accelerations[i] = 1 / micro * (internalForces[i] + externalForces[i]);
			velocities[i] = velocities[i] + accelerations[i] * tiemstep;
			positions[i] = positions[i] + velocities[i] * tiemstep;
			varVector += positions[i];
		}
		varVector /= numberOfMasses;

		for (int i = 0; i < numberOfMasses; i++)
		{
			variation += glm::length2(positions[i] - varVector) ;
		}
		variation /= numberOfMasses;

		iter++;
	}

	updatePositions();
}

void SimulationManager::shutdown()
{
}

glm::vec3 SimulationManager::getInternalForce(int index)
{
	glm::vec3 internalForce = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<Spring> springs = massPoints[index].getSprings();

	float stiffness = 1;

	for(Spring spring : springs)
	{
		glm::vec3 pointA = positions[spring.indexA];
		glm::vec3 pointB = positions[spring.indexB];

		glm::vec3 l = pointB - pointA;
		float springLength = glm::length(pointA - pointB);

		internalForce += stiffness * (
			l - (spring.restLength * glm::normalize(l))
			);
	}

	return internalForce;
}

glm::vec3 SimulationManager::getExternalForce(int index)
{
	glm::vec3 g = glm::vec3(0.0f, -9.81f, 0.0f);
	glm::vec3 pointVelocity = velocities[index];
	float dampingCoefficient = 0.1f;

	float viscosityCoefficient = 0.1f;
	glm::vec3 fluidVelocity = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 unitNormal = glm::vec3(0.0f, 0.0f, -1.0f);

	return massPoints[index].getMass() * g
		- dampingCoefficient * pointVelocity
		+ viscosityCoefficient * (glm::dot(unitNormal,(fluidVelocity) - pointVelocity)) * unitNormal;
}


void SimulationManager::updatePositions()
{
	for (int i = 0; i < 100; i++)
	{
		massPoints[i].setPosition(positions[i]);
		massPoints[i].setVelocity(velocities[i]);
		massPoints[i].setAcceleration(accelerations[i]);
	}
}
