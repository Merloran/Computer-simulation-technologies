#pragma once
#include "mass_point.hpp"


class SimulationManager
{
public:
	SimulationManager(SimulationManager&) = delete;
	static SimulationManager& get();

	static constexpr int rows = 10;
	static constexpr int cols = 10;

	void startup();

	void update(Float32 dT);

	MassPoint *get_mass_points();

	void shutdown();


private:
	SimulationManager() = default;
	~SimulationManager() = default;

	static constexpr int numberOfMasses = rows * cols;
	MassPoint massPoints[numberOfMasses];

	glm::vec3 positions[numberOfMasses];
	glm::vec3 velocities[numberOfMasses];
	glm::vec3 accelerations[numberOfMasses];

	glm::vec3 internalForces[numberOfMasses];
	glm::vec3 externalForces[numberOfMasses];

	float variationThreshold = 0.01f;
	int minIterations = 5;

	glm::vec3 getInternalForce(int index);
	glm::vec3 getExternalForce(int index);

	void updatePositions();
};

