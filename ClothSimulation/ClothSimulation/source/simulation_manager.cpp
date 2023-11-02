#include "simulation_manager.hpp"

#include <imgui.h>

SimulationManager& SimulationManager::get()
{
	static SimulationManager instance;
	return instance;
}

void SimulationManager::startup()
{
	maximumTimeStep = glm::pi<Float32>() * glm::sqrt(mass / stiffness);
	initialLength = newInitialLength;
	gridSize = newGridSize;
	const Int32 numberOfMasses = glm::max(gridSize.x * gridSize.y, 0);
	const Int32 numberOfSprings = glm::max(6 * numberOfMasses - 5 * (gridSize.y + gridSize.x) + 2, 0);

	massPoints.reserve(numberOfMasses);
	springs.reserve(numberOfSprings);
	internalForces.resize(numberOfMasses, glm::vec3(0.0f));
	externalForces.resize(numberOfMasses, glm::vec3(0.0f));

	for (Int32 y = 0; y < gridSize.y; ++y)
	{
		for (Int32 x = 0; x < gridSize.x; ++x)
		{
			glm::vec3 position = {
									initialLength * Float32(x),
									initialLength * Float32(-y),
									0.0f
								 };

			massPoints.emplace_back(mass, position);
		}
	}
	massPoints[0].isSimulated = false;
	massPoints[gridSize.x - 1].isSimulated = false;

	for (Int32 y = 0; y < gridSize.y; ++y)
	{
		for (Int32 x = 0; x < gridSize.x; ++x)
		{
			process_mass_point(x, y);
		}
	}

	shouldReset = false;
}

void SimulationManager::update(Float32 dT)
{
	dT = glm::min(dT, maximumTimeStep * 0.1f);
	if (shouldReset)
	{
		shutdown();
	}

	if (!isSimulating)
	{
		return;
	}

	reset_forces();
	for (Int32 i = 0; i < springs.size(); ++i)
	{
		compute_internal_force(i);
	}

	for (Int32 i = 0; i < massPoints.size(); ++i)
	{
		compute_external_force(i);
	}

	float variation = 0.f;
	for (Int32 i = 0; i < minIterations || false; ++i) //variation > variationThreshold || 
	{
		variation = 0.f;
		glm::vec3 varVector = glm::vec3(0.0f);

		for (Int32 j = 0; j < massPoints.size(); ++j)
		{
			MassPoint &massPoint = massPoints[j];
			if (!massPoint.isSimulated)
			{
				continue;
			}
			massPoint.acceleration = 1.0f / massPoint.mass * (internalForces[j] + externalForces[j]);
			massPoint.velocity += massPoint.acceleration * dT;
			massPoint.position += massPoint.velocity * dT;
			varVector += massPoint.position;
		}
		
		varVector /= massPoints.size();

		for (Int32 j = 0; j < massPoints.size(); j++)
		{
			variation += glm::length2(massPoints[j].position - varVector);
		}
		variation /= massPoints.size();
	}
}

const std::vector<MassPoint>& SimulationManager::get_mass_points() const
{
	return massPoints;
}

const std::vector<Spring>& SimulationManager::get_springs() const
{
	return springs;
}

Float32 SimulationManager::get_initial_length() const
{
	return initialLength;
}

void SimulationManager::show_gui()
{
	ImGui::Begin("Simulation settings");

	ImGui::DragFloat3("Fluid Velocity", &fluidVelocity[0], 0.01f, 0.0f, 100.0f, "%.2f");
	ImGui::DragFloat3("Gravity", &gravity[0], 0.01f, -30.0f, 30.0f, "%.2f");
	ImGui::DragFloat("Stiffness", &stiffness, 0.1f, 1.0f, 1000.0f, "%.1f");
	ImGui::DragFloat("Mass", &mass, 0.001f, 0.001f, 10.0f, "%.3f");
	ImGui::DragFloat("Initial Length", &newInitialLength, 0.1f, 0.1f, 10.0f, "%.1f");
	ImGui::DragFloat("Damping", &damping, 0.01f, 0.01f, 1.0f, "%.2f");
	ImGui::DragFloat("Viscosity", &viscosity, 0.01f, 0.0f, 2.0f, "%.2f");
	ImGui::DragInt2("Grid Size", &newGridSize[0], 1, 1, 100);

	shouldReset = ImGui::Button("Reset");
	ImGui::Checkbox("Simulate", &isSimulating);


	ImGui::End();
}

void SimulationManager::shutdown()
{
	massPoints.clear();
	springs.clear();
	internalForces.clear();
	externalForces.clear();
	if (shouldReset)
	{
		startup();
	}
}

void SimulationManager::process_mass_point(Int32 x, Int32 y)
{
	const Int32 indexA = x + y * gridSize.x;
	Int32 indexB;
	
	MassPoint &massA = massPoints[indexA];
	//flexion springs
	if (x + 2 < gridSize.x) 
	{
		indexB = indexA + 2;
		MassPoint &massB = massPoints[indexB];
		const Float32 length = glm::length(massA.position - massB.position);
		springs.emplace_back(length, indexA, indexB, stiffness);
	}
	if (y + 2 < gridSize.y) 
	{
		indexB = indexA + 2 * gridSize.x;
		MassPoint &massB = massPoints[indexB];
		const Float32 length = glm::length(massA.position - massB.position);
		springs.emplace_back(length, indexA, indexB, stiffness);
	}

	//shear springs
	if (x - 1 >= 0 && y + 1 < gridSize.y) 
	{
		indexB = indexA + gridSize.x - 1;
		MassPoint &massB = massPoints[indexB];
		const Float32 length = glm::length(massA.position - massB.position);
		springs.emplace_back(length, indexA, indexB, stiffness);
	}
	if (x + 1 < gridSize.x && y + 1 < gridSize.y) 
	{
		indexB = indexA + gridSize.x + 1;
		MassPoint &massB = massPoints[indexB];
		const Float32 length = glm::length(massA.position - massB.position);
		springs.emplace_back(length, indexA, indexB, stiffness);
	}

	//structural springs
	if (x + 1 < gridSize.x) 
	{
		indexB = indexA + 1;
		MassPoint &massB = massPoints[indexB];
		const Float32 length = glm::length(massA.position - massB.position);
		springs.emplace_back(length, indexA, indexB, stiffness);
	}
	if (y + 1 < gridSize.y) 
	{
		indexB = indexA + gridSize.x;
		MassPoint &massB = massPoints[indexB];
		const Float32 length = glm::length(massA.position - massB.position);
		springs.emplace_back(length, indexA, indexB, stiffness);
	}
}

void SimulationManager::compute_internal_force(Int32 springIndex)
{
	const Spring &spring = springs[springIndex];

	const glm::vec3& pointA = massPoints[spring.indexA].position;
	const glm::vec3& pointB = massPoints[spring.indexB].position;
	const glm::vec3 l = pointB - pointA;

	const glm::vec3 force = spring.stiffness * (l - spring.restLength * glm::normalize(l));
	internalForces[spring.indexA] += force;
	internalForces[spring.indexB] -= force;
}

void SimulationManager::compute_external_force(Int32 massIndex)
{
	MassPoint &massPoint = massPoints[massIndex];
	glm::vec3 unitNormal = glm::vec3(0.0f, 0.0f, 1.0f);

	externalForces[massIndex] = massPoint.mass * gravity
	- damping * massPoint.velocity
	+ (viscosity * glm::dot(unitNormal,fluidVelocity - massPoint.velocity)) * unitNormal;
}

void SimulationManager::reset_forces()
{
	for (glm::vec3& force : internalForces)
	{
		force = { 0.0f, 0.0f, 0.0f };
	}
}