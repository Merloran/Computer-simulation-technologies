#include "simulation_manager.hpp"

SimulationManager& SimulationManager::get()
{
	static SimulationManager instance;
	return instance;
}

void SimulationManager::startup()
{
	for (Int32 i = 0; i < numberOfMasses; ++i)
	{
		Int32 x = i % cols, y = Int32(i / rows);
		const Float32 initialLength = 2.0f;
		massPoints[i] = MassPoint(1.0f, glm::vec3(initialLength * Float32(x),
												  initialLength * Float32(y),
												  0.0f),
										glm::vec3(0.0f),
									    glm::vec3(0.0f));
	} // checked
	for (Int32 i = 0; i < numberOfMasses; ++i)
	{
		Int32 x = i % cols, y = Int32(i / rows);
		massPoints[i].setSprings(x, y, cols, rows);
	} // checked
}

void SimulationManager::update(Float32 dT)
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
	} //checked?

	int iter = 0;
	float micro = 0.1f;

	float variation = 0.f;

	while (iter < minIterations) { //variation > variationThreshold || 

		variation = 0.f;
		glm::vec3 varVector = glm::vec3(0.0f);

		for (int i = 0; i < numberOfMasses; i++)
		{
			accelerations[i] = 1.0f / micro * (internalForces[i] + externalForces[i]);
			velocities[i] = velocities[i] + accelerations[i] * dT;
			positions[i] = positions[i] + velocities[i] * dT;
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

MassPoint* SimulationManager::get_mass_points()
{
	return massPoints;
}

void SimulationManager::shutdown()
{
}

glm::vec3 SimulationManager::getInternalForce(int index)
{
	glm::vec3 internalForce = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<Spring> springs = massPoints[index].getSprings();

	float stiffness = 10.0f;

	for(Spring& spring : springs)
	{
		glm::vec3 pointA = positions[index];
		glm::vec3 pointB = positions[spring.indexB];

		glm::vec3 l = pointB - pointA;

		internalForce += stiffness * (l - (spring.restLength * glm::normalize(l)));
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
		if (i == 0 || i == 9)
		{
			continue;
		}
		massPoints[i].setPosition(positions[i]);
		massPoints[i].setVelocity(velocities[i]);
		massPoints[i].setAcceleration(accelerations[i]);
	}
}
