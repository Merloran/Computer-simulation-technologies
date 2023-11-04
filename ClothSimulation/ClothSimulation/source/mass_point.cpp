#include "mass_point.hpp"
#include "simulation_manager.hpp"


MassPoint::MassPoint(float mass, const glm::vec3& position, const glm::vec3& normal)
	: mass(mass)
	, position(position)
	, normal(normal)
	, velocity(0.0f)
	, acceleration(0.0f)
	, isSimulated(true)
{}