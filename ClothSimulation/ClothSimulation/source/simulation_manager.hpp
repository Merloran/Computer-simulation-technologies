#pragma once
#include "mass_point.hpp"


class SimulationManager
{
public:
	SimulationManager(SimulationManager&) = delete;
	static SimulationManager& get();

	void startup();

	void update(Float32 dT);
	
	const std::vector<MassPoint>& get_mass_points() const;
	const std::vector<Spring>& get_springs() const;
	Float32 get_initial_length() const;
	void show_gui();

	void shutdown();


private:
	SimulationManager() = default;
	~SimulationManager() = default;

	glm::ivec2 gridSize = { 10, 10 };
	glm::ivec2 newGridSize = gridSize;
	std::vector<MassPoint>	massPoints;
	std::vector<glm::vec3>	predictedPositions;
	std::vector<Spring>		springs;
	std::vector<glm::vec3>	internalForces;
	std::vector<glm::vec3>	externalForces;

	void process_mass_point(Int32 x, Int32 y);

	int minIterations = 1;
	float variationThreshold = 0.1f;
	glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };
	glm::vec3 fluidVelocity = { 0.0f, 0.0f, 30.0f };
	Float32 stiffness = 100.0f;
	Float32 mass = 1.0f;
	Float32 viscosity = 1.0f;
	Float32 damping = 0.1f;
	Float32 initialLength = 2.0f;
	Float32 newInitialLength = initialLength;
	Float32 maximumTimeStep;
	bool isSimulating = false;
	bool shouldReset = false;


	void compute_internal_force(Int32 springIndex);
	void compute_external_force(Int32 massIndex);
	void reset_forces();
	[[deprecated("This method does not work, probably because we are using perfect wind")]]
	void update_normals();
};

