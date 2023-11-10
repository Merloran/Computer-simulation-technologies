#pragma once
template<typename Type>
struct Handle;
struct ClothData;
struct Mesh;

class SimulationManager
{
public:
	SimulationManager(SimulationManager&) = delete;
	static SimulationManager& get();

	void startup();
	void update();

	const ClothData& get_cloth_data(Int32 index) const;
	bool is_debug_mode() const;
	void create_soft_mesh(const std::string& name, const glm::ivec2& gridSize,
						  const glm::vec2& meshSize, Float32 clothMass, Float32 stiffness);
	void show_gui();

	void shutdown();


private:
	SimulationManager() = default;
	~SimulationManager() = default;
	
	std::vector<ClothData> cloths;

	glm::ivec2 gridSize = { 10, 10 };
	glm::ivec2 newGridSize = gridSize;

	std::vector<glm::vec3>	internalForces;
	std::vector<glm::vec3>	externalForces;
	std::vector<glm::vec3>	predictedPositions;

	int minIterations = 1;
	float variationThreshold = 0.1f;
	glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };
	glm::vec3 fluidVelocity = { 0.0f, 0.0f, 30.0f };
	Float32 stiffness = 100.0f;
	Float32 clothMass = 100.0f;
	Float32 viscosity = 1.0f;
	Float32 damping = 0.1f;
	glm::vec2 meshSize = { 20.0f, 20.0f };
	Float32 deltaTime = 0.016f;
	bool isSimulating = false;
	bool shouldReset = false;
	bool isDebugMode = false;

	
	void compute_internal_forces(const Mesh &mesh, const ClothData &clothData);
	void compute_external_forces(const ClothData &clothData);
	void calculate_positions(Mesh& mesh, const ClothData &clothData, const glm::vec2& initialLengths);
	void calculate_indexes(Mesh& mesh, const ClothData& clothData);
	void calculate_uvs(Mesh &mesh, const ClothData &clothData);
	void update_normals(Mesh& mesh);
	void calculate_springs(const Mesh& mesh, ClothData &clothData);
};

