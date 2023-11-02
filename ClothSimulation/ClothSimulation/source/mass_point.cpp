#include "mass_point.hpp"
#include "simulation_manager.hpp"


MassPoint::MassPoint(float mass, glm::vec3 position)
	: mass(mass)
	, position(position)
	, velocity(0.0f)
	, acceleration(0.0f)
	, isSimulated(true)
{}