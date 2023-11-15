#include "simulation_manager.hpp"

#include <imgui.h>
#include <filesystem>

#include "resource_manager.hpp"
#include "Common/mesh.hpp"
#include "Common/handle.hpp"
#include "Common/cloth_data.hpp"
#include "Common/material.hpp"
#include "Common/model.hpp"
#include "Common/texture.hpp"

SimulationManager& SimulationManager::get()
{
	static SimulationManager instance;
	return instance;
}

void SimulationManager::startup()
{
	SResourceManager &resourceManager = SResourceManager::get();

	create_soft_mesh("Flag", gridSize, meshSize, clothMass, stiffness);

	ClothData &clothData = cloths[cloths.size() - 1];


	Material material;
	material.albedo = resourceManager.load_texture(resourceManager.TEXTURES_PATH + "Silence/Second.jpg",
												   "SilenceAlbedo", ETextureType::Albedo);
	resourceManager.create_material(material, "Silence");
	Texture &albedo = resourceManager.get_texture_by_handle(material.albedo);
	resourceManager.generate_opengl_texture(albedo);

	Model model;
	model.meshes.emplace_back(clothData.simulatedMesh);
	model.materials.emplace_back(resourceManager.get_material_handle_by_name("Silence"));

	resourceManager.create_model(model, "Flag");
	resourceManager.generate_opengl_model(model);

	shouldReset = false;
}

void SimulationManager::update()
{
	if (shouldReset)
	{
		shutdown();
	}

	if (!isSimulating)
	{
		return;
	}

	SResourceManager &resourceManager = SResourceManager::get();
	ClothData &clothData = cloths[0];
	Mesh &mesh = resourceManager.get_mesh_by_handle(clothData.simulatedMesh);

	compute_external_forces(clothData);
	Float32 variation = 0.0f;
	glm::vec3 current(0.0f), predicted(0.0f);

	Int32 stableCounter = 0;
	for (Int32 i = 0; i < mesh.positions.size(); ++i)
	{
		if (!clothData.simulatedFlags[i])
		{
			stableCounter++;
			continue;
		}
		predicted += mesh.positions[i];
	}
	predicted /= Float32(mesh.positions.size() - stableCounter);

	for (Int32 i = 0; i < minIterations || variation > variationThreshold; ++i) 
	{
		current = predicted;
		predicted = glm::vec3(0.0f);
		compute_internal_forces(mesh, clothData);
		stableCounter = 0;
		for (Int32 j = 0; j < mesh.positions.size(); ++j)
		{
			if (!clothData.simulatedFlags[j])
			{
				stableCounter++;
				continue;
			}
			clothData.accelerations[j] = (internalForces[j] + externalForces[j]) / clothData.masses[j];
			clothData.velocities[j]   += clothData.accelerations[j] * deltaTime;
			mesh.positions[j]		  += clothData.velocities[j] * deltaTime;
			predicted += mesh.positions[j];
		}
		predicted /= Float32(mesh.positions.size() - stableCounter);
		variation = glm::abs(glm::length(predicted) - glm::length(current));
	}
	update_normals(mesh);
	resourceManager.update_opengl_model(resourceManager.get_model_by_name("Flag"));
}

const ClothData& SimulationManager::get_cloth_data(Int32 index) const
{
	if (index < 0 || index >= cloths.size())
	{
		SPDLOG_WARN("Invalid cloth data index.");
		return ClothData();
	}
	return cloths[index];
}

bool SimulationManager::is_debug_mode() const
{
	return isDebugMode;
}

void SimulationManager::create_soft_mesh(const std::string &name, const glm::ivec2 &gridSize,
                                         const glm::vec2 &meshSize, Float32 clothMass, Float32 stiffness)
{
	SResourceManager &resourceManager = SResourceManager::get();
	std::vector<Mesh> meshes = resourceManager.get_meshes();
	cloths.emplace_back();
	ClothData &clothData = cloths[cloths.size() - 1];
	clothData.simulatedMesh = resourceManager.create_mesh(name);
	Mesh &mesh = resourceManager.get_mesh_by_handle(clothData.simulatedMesh);

	const glm::vec2 initialLengths = { meshSize.x / Float32(gridSize.x - 1), meshSize.y / Float32(gridSize.y - 1) };
	const Int32 numberOfMasses = glm::max(gridSize.x * gridSize.y, 0);
	const Int32 numberOfSprings = glm::max(6 * numberOfMasses - 5 * (gridSize.y + gridSize.x) + 2, 0);
	const Int32 numberOfIndexes = glm::max((gridSize.x - 1) * (gridSize.y - 1) * 6, 0);
	const Float32 massOfPoint = clothMass / Float32(numberOfMasses);

	// Reserve mass points
	clothData.gridSize = gridSize;
	clothData.accelerations.resize(numberOfMasses, glm::vec3(0.0f));
	clothData.velocities.resize(numberOfMasses, glm::vec3(0.0f));
	clothData.masses.resize(numberOfMasses, massOfPoint);
	clothData.simulatedFlags.resize(numberOfMasses, true);
	// Reserve springs
	clothData.restLengths.reserve(numberOfSprings);
	clothData.stiffnesses.resize(numberOfSprings, stiffness);
	clothData.springAttachments.reserve(numberOfSprings);
	// Reserve mesh
	mesh.positions.reserve(numberOfMasses);
	mesh.normals.resize(numberOfMasses, glm::vec3(0.0f));
	mesh.uvs.reserve(numberOfMasses);
	mesh.indexes.reserve(numberOfIndexes);
	// Reserve forces
	internalForces.resize(numberOfMasses, glm::vec3(0.0f));
	externalForces.resize(numberOfMasses, glm::vec3(0.0f));
	// Init positions
	calculate_positions(mesh, clothData, initialLengths);
	calculate_indexes(mesh, clothData);
	calculate_uvs(mesh, clothData);
	update_normals(mesh);

	clothData.simulatedFlags[0] = false;
	clothData.simulatedFlags[gridSize.x * Int32(gridSize.y * 0.5f)] = false;
	clothData.simulatedFlags[gridSize.x * (gridSize.y - 1)] = false;

	calculate_springs(mesh, clothData);
}

void SimulationManager::show_gui()
{
	ImGui::Begin("Simulation settings");

	ImGui::DragFloat3("Fluid Velocity", &fluidVelocity[0], 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::DragFloat3("Gravity", &gravity[0], 0.01f, -30.0f, 30.0f, "%.2f");
	ImGui::DragFloat("Stiffness", &stiffness, 0.1f, 1.0f, 1000.0f, "%.1f");
	ImGui::DragFloat("Cloth mass", &clothMass, 0.1f, 1.0f, 1000.0f, "%.1f");
	ImGui::DragFloat("Damping", &damping, 0.01f, 0.01f, 1.0f, "%.2f");
	ImGui::DragFloat("Viscosity", &viscosity, 0.01f, 0.0f, 2.0f, "%.2f");
	ImGui::DragFloat2("Mesh size", &meshSize[0], 0.1f, 0.1f, 200.0f, "%.1f");
	ImGui::DragInt2("Grid Size", &gridSize[0], 1, 1, 30);
	ImGui::DragFloat("Time step", &deltaTime, 0.0001f, 0.0f, 0.05f, "%.4f");

	shouldReset = ImGui::Button("Reset");
	ImGui::Checkbox("Simulate", &isSimulating);
	ImGui::Checkbox("Debug mode", &isDebugMode);
	ImGui::Text("FPS: %.2f, %.2fms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::End();
}

void SimulationManager::shutdown()
{
	cloths.clear();
	internalForces.clear();
	externalForces.clear();
	if (shouldReset)
	{
		SResourceManager &resourceManager = SResourceManager::get();
		resourceManager.shutdown();
		resourceManager.startup();
		startup();
	}
}


void SimulationManager::compute_internal_forces(const Mesh& mesh, const ClothData& clothData)
{
	for (glm::vec3 &force : internalForces)
	{
		force = { 0.0f, 0.0f, 0.0f };
	}

	for (Int32 i = 0; i < clothData.restLengths.size(); ++i)
	{
		const Int32 indexA = clothData.springAttachments[i].x;
		const Int32 indexB = clothData.springAttachments[i].y;

		const glm::vec3 &pointA = mesh.positions[indexA];
		const glm::vec3 &pointB = mesh.positions[indexB];
		const glm::vec3 l = pointB - pointA;
		if (glm::length2(l) <= glm::epsilon<Float32>())
		{
			continue;
		}

		const glm::vec3 force = clothData.stiffnesses[i] * (l - clothData.restLengths[i]  * glm::normalize(l));
		internalForces[indexA] += force;
		internalForces[indexB] -= force;
	}
}

void SimulationManager::compute_external_forces(const ClothData &clothData)
{

	glm::vec3 normal(0.0f);
	if (glm::length2(fluidVelocity) > 0.0f)
	{
		normal = -glm::normalize(fluidVelocity);
	}

	for (Int32 i = 0; i < clothData.masses.size(); ++i)
	{
		const glm::vec3 gravityForce = clothData.masses[i] * gravity;
		const glm::vec3 dampingForce = -damping * clothData.velocities[i];
		glm::vec3 fluidForce = viscosity
							 * glm::dot(normal, fluidVelocity - clothData.velocities[i])
							 * normal;

		externalForces[i] = gravityForce + dampingForce + fluidForce;
	}
}

void SimulationManager::calculate_positions(Mesh& mesh, const ClothData &clothData, const glm::vec2& initialLengths)
{
	const glm::ivec2 &gridSize = clothData.gridSize;
	for (Int32 y = 0; y < gridSize.y; ++y)
	{
		for (Int32 x = 0; x < gridSize.x; ++x)
		{
			mesh.positions.emplace_back(initialLengths.x * Float32(x),
										initialLengths.y * Float32(-y),
										0.0f);
		}
	}
}

//		B
//      *
//     /|
//    / |
//   /  |A
// C*---*---*C
//      |  /
//      | /
//      |/
//      *
//		B
void SimulationManager::calculate_indexes(Mesh& mesh, const ClothData& clothData)
{
	const glm::ivec2& gridSize = clothData.gridSize;
	for (Int32 y = 0; y < gridSize.y; ++y)
	{
		for (Int32 x = 0; x < gridSize.x; ++x)
		{
			const Int32 indexA = y * gridSize.x + x;
			if (x - 1 >= 0 && y - 1 >= 0) // Upper triangle
			{
				const Int32 indexB = (y - 1) * gridSize.x + x;
				const Int32 indexC = y * gridSize.x + x - 1;
				mesh.indexes.emplace_back(indexA);
				mesh.indexes.emplace_back(indexB);
				mesh.indexes.emplace_back(indexC);
			}

			if (x + 1 < gridSize.x && y + 1 < gridSize.y) // Lower triangle
			{
				const Int32 indexB = (y + 1) * gridSize.x + x;
				const Int32 indexC = y * gridSize.x + x + 1;
				mesh.indexes.emplace_back(indexA);
				mesh.indexes.emplace_back(indexB);
				mesh.indexes.emplace_back(indexC);
			}
		}
	}
}

void SimulationManager::update_normals(Mesh &mesh)
{
	for (Int32 i = 0; i < mesh.indexes.size(); i += 3)
	{
		const UInt32 indexA = mesh.indexes[i];
		const UInt32 indexB = mesh.indexes[i + 1];
		const UInt32 indexC = mesh.indexes[i + 2];
		const glm::vec3 ba = mesh.positions[indexB] - mesh.positions[indexA];
		const glm::vec3 ca = mesh.positions[indexC] - mesh.positions[indexA];
		const glm::vec3 normal = glm::normalize(glm::cross(ba, ca));

		mesh.normals[indexA] += normal;
		mesh.normals[indexB] += normal;
		mesh.normals[indexC] += normal;
	}

	for (glm::vec3& normal : mesh.normals)
	{
		normal = glm::normalize(normal);
	}
}

void SimulationManager::calculate_uvs(Mesh& mesh, const ClothData &clothData)
{
	const glm::ivec2 &gridSize = clothData.gridSize;
	const glm::vec2 uvOffset = 1.0f / glm::vec2(gridSize - 1);
	for (Int32 y = 0; y < gridSize.y; ++y)
	{
		for (Int32 x = 0; x < gridSize.x; ++x)
		{
			mesh.uvs.emplace_back(uvOffset.x * x, uvOffset.y * y);
		}
	}

}

void SimulationManager::calculate_springs(const Mesh &mesh, ClothData &clothData)
{
	const glm::ivec2 &gridSize = clothData.gridSize;
	for (Int32 y = 0; y < gridSize.y; ++y)
	{
		for (Int32 x = 0; x < gridSize.x; ++x)
		{
			const Int32 indexA = x + y * gridSize.x;
			Int32 indexB;
			Float32 length;
			
			//flexion springs
			if (x + 2 < gridSize.x)
			{
				indexB = indexA + 2;
				length = glm::length(mesh.positions[indexA] - mesh.positions[indexB]);
				clothData.restLengths.emplace_back(length);
				clothData.springAttachments.emplace_back(indexA, indexB);
			}
			if (y + 2 < gridSize.y)
			{
				indexB = indexA + 2 * gridSize.x;
				length = glm::length(mesh.positions[indexA] - mesh.positions[indexB]);
				clothData.restLengths.emplace_back(length);
				clothData.springAttachments.emplace_back(indexA, indexB);
			}

			//shear springs
			if (x - 1 >= 0 && y + 1 < gridSize.y)
			{
				indexB = indexA + gridSize.x - 1;
				length = glm::length(mesh.positions[indexA] - mesh.positions[indexB]);
				clothData.restLengths.emplace_back(length);
				clothData.springAttachments.emplace_back(indexA, indexB);
			}
			if (x + 1 < gridSize.x && y + 1 < gridSize.y)
			{
				indexB = indexA + gridSize.x + 1;
				length = glm::length(mesh.positions[indexA] - mesh.positions[indexB]);
				clothData.restLengths.emplace_back(length);
				clothData.springAttachments.emplace_back(indexA, indexB);
			}

			//structural springs
			if (x + 1 < gridSize.x)
			{
				indexB = indexA + 1;
				length = glm::length(mesh.positions[indexA] - mesh.positions[indexB]);
				clothData.restLengths.emplace_back(length);
				clothData.springAttachments.emplace_back(indexA, indexB);
			}
			if (y + 1 < gridSize.y)
			{
				indexB = indexA + gridSize.x;
				length = glm::length(mesh.positions[indexA] - mesh.positions[indexB]);
				clothData.restLengths.emplace_back(length);
				clothData.springAttachments.emplace_back(indexA, indexB);
			}
		}
	}
}
