#pragma once
template<typename Type>
struct Handle;
struct Mesh;

struct ClothData //Something like cloth component that require mesh
{
	// Mass points data
	std::vector<glm::vec3>	velocities;
	std::vector<glm::vec3>	accelerations;
	std::vector<Float32>	masses;
	std::vector<bool>		simulatedFlags;  // True means it is simulated, False it's attached
	glm::ivec2				gridSize;

	// Springs data
	std::vector<Float32>    restLengths;
	std::vector<Float32>    stiffnesses;
	std::vector<glm::ivec2> springAttachments;

	Handle<Mesh>			simulatedMesh;
};
