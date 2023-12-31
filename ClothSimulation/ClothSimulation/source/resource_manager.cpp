#define __STDC_LIB_EXT1__ 1
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#include "resource_manager.hpp"

#include "Common/handle.hpp"
#include "Common/model.hpp"
#include "Common/material.hpp"
#include "Common/mesh.hpp"
#include "Common/texture.hpp"

#include <filesystem>
#include <glad/glad.h>

void SResourceManager::startup()
{
	SPDLOG_INFO("Resource Manager startup.");
	Material defaultMaterial;
	defaultMaterial.albedo			 = load_texture(TEXTURES_PATH + "Default/Albedo.png", "DefaultBaseColor", ETextureType::Albedo);
	defaultMaterial.normal			 = load_texture(TEXTURES_PATH + "Default/Normal.png", "DefaultNormal", ETextureType::Normal);
	defaultMaterial.roughness		 = load_texture(TEXTURES_PATH + "Default/Roughness.png", "DefaultRoughness", ETextureType::Roughness);
	defaultMaterial.metalness		 = load_texture(TEXTURES_PATH + "Default/Metalness.png", "DefaultMetalness", ETextureType::Metalness);
	defaultMaterial.ambientOcclusion = load_texture(TEXTURES_PATH + "Default/AmbientOcclusion.png", "DefaultAmbientOcclusion", ETextureType::AmbientOcclusion);
	create_material(defaultMaterial, "DefaultMaterial");
}

SResourceManager& SResourceManager::get()
{
	static SResourceManager instance;
	return instance;
}

void SResourceManager::load_gltf_asset(const std::string& filePath)
{
	load_gltf_asset(std::filesystem::path(filePath));
}

void SResourceManager::load_gltf_asset(const std::filesystem::path & filePath)
{
	tinygltf::Model gltfModel;
	std::string error;
	std::string warning;

	tinygltf::TinyGLTF loader;

	if (!loader.LoadASCIIFromFile(&gltfModel, &error, &warning, filePath.string()) || !warning.empty() || !error.empty())
	{
		SPDLOG_ERROR("Failed to load gltf file: {} - {} - {}", filePath.string(), error, warning);
		return;
	}

	for (tinygltf::Mesh& gltfMesh : gltfModel.meshes)
	{
		load_model(filePath, gltfMesh, gltfModel);
	}
}

void SResourceManager::generate_opengl_texture(Texture& texture)
{
	glGenTextures(1, &texture.gpuId);
	glBindTexture(GL_TEXTURE_2D, texture.gpuId);

	if (texture.channels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 
				     0, 
					 GL_RGB, 
					 texture.size.x, 
					 texture.size.y,
					 0, 
					 GL_RGB,
					 GL_UNSIGNED_BYTE,
					 texture.data);
	}
	else if (texture.channels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 texture.size.x,
					 texture.size.y,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 texture.data);
	} else {
		SPDLOG_WARN("Not supported count of channels: {}", texture.channels);
		glDeleteTextures(1, &texture.gpuId);
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SResourceManager::generate_opengl_model(Model& model)
{
	for (const Handle<Mesh>& handle : model.meshes)
	{
		Mesh& mesh = get_mesh_by_handle(handle);

		glGenVertexArrays(1, &mesh.gpuIds[0]);
		glGenBuffers(1, &mesh.gpuIds[1]);
		glGenBuffers(1, &mesh.gpuIds[2]);

		glBindVertexArray(mesh.gpuIds[0]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.gpuIds[2]);

		const Int64 positionsSize = mesh.positions.size() * sizeof(glm::vec3);
		const Int64 normalsSize	  = mesh.normals.size() * sizeof(glm::vec3);
		const Int64 uvsSize		  = mesh.uvs.size() * sizeof(glm::vec2);

		glBufferData(GL_ARRAY_BUFFER, positionsSize + normalsSize + uvsSize, nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positionsSize, mesh.positions.data());
		glBufferSubData(GL_ARRAY_BUFFER, positionsSize, normalsSize, mesh.normals.data());
		glBufferSubData(GL_ARRAY_BUFFER, positionsSize + normalsSize, uvsSize, mesh.uvs.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.gpuIds[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indexes.size() * sizeof(UInt32), mesh.indexes.data(), GL_STATIC_DRAW);

		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		// Normal attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)positionsSize);
		// Texture position attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(positionsSize + normalsSize));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void SResourceManager::update_opengl_model(Model& model)
{
	for (const Handle<Mesh> &handle : model.meshes)
	{
		Mesh &mesh = get_mesh_by_handle(handle);

		const Int64 positionsSize = mesh.positions.size() * sizeof(glm::vec3);
		const Int64 normalsSize = mesh.normals.size() * sizeof(glm::vec3);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.gpuIds[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positionsSize, mesh.positions.data());
		glBufferSubData(GL_ARRAY_BUFFER, positionsSize, normalsSize, mesh.normals.data());
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

Handle<Model> SResourceManager::load_model(const std::filesystem::path & filePath, tinygltf::Mesh &gltfMesh, tinygltf::Model &gltfModel)
{
	if (nameToIdModels.find(gltfMesh.name) != nameToIdModels.end())
	{
		SPDLOG_ERROR("Model with name {} already exist!", gltfMesh.name);
		return Handle<Model>::sNone;
	}

	models.emplace_back();
	const Int64 modelId = models.size() - 1;
	Model &model = models[modelId];

	model.meshes.reserve(gltfMesh.primitives.size());
	model.directory = filePath.string();

	for (Int32 i = 0; i < gltfMesh.primitives.size(); i++)
	{
		tinygltf::Primitive& primitive = gltfMesh.primitives[i];
		std::string meshName = gltfMesh.name + std::to_string(i);
		Handle<Mesh> mesh = load_mesh(meshName, primitive, gltfModel);
		model.meshes.push_back(mesh);
		Handle<Material> material = get_material_handle_by_name("DefaultMaterial");
		if (primitive.material >= 0)
		{
			material = load_material(filePath.parent_path(), 
									 gltfModel.materials[primitive.material], 
									 gltfModel);
		}
		model.materials.push_back(material);
	}

	Handle<Model> modelHandle{ Int32(modelId) };
	nameToIdModels[gltfMesh.name] = modelHandle;

	return modelHandle;
}

Handle<Mesh> SResourceManager::load_mesh(const std::string& meshName, tinygltf::Primitive& primitive, tinygltf::Model& gltfModel)
{
	if (nameToIdMeshes.find(meshName) != nameToIdMeshes.end())
	{
		SPDLOG_ERROR("Mesh with name {} already exist!", meshName);
		return Handle<Mesh>::sNone;
	}

	meshes.emplace_back();
	const Int64 meshId = meshes.size() - 1;
	Mesh &mesh   = meshes[meshes.size() - 1];

	const tinygltf::Accessor& indexesAccessor = gltfModel.accessors[primitive.indices];
	Int32 indexesType						  = indexesAccessor.componentType;
	Int32 indexesTypeCount					  = indexesAccessor.type;

	// Load indexes
	switch (indexesType) {
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: 
		{
			process_accessor<UInt16>(gltfModel, indexesAccessor, mesh.indexes);
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_SHORT: 
		{
			process_accessor<Int16>(gltfModel, indexesAccessor, mesh.indexes);
			break;
		}
		default:
		{
			SPDLOG_ERROR("Mesh indexes not loaded, not supported type: GLTF_COMPONENT_TYPE {}; Name {}", indexesType, meshName);
			meshes.pop_back();
			return Handle<Mesh>::sNone;
		}	
	}


	// Load positions
	const tinygltf::Accessor& positionsAccessor = gltfModel.accessors[primitive.attributes["POSITION"]];
	Int32 positionsType							= positionsAccessor.componentType;
	Int32 positionsTypeCount					= positionsAccessor.type;

	if (positionsTypeCount == TINYGLTF_TYPE_VEC3) 
	{
		if (positionsType == TINYGLTF_COMPONENT_TYPE_FLOAT) 
		{
			process_accessor<glm::vec3>(gltfModel, positionsAccessor, mesh.positions);
		} else {
			SPDLOG_ERROR("Mesh positions not loaded, not supported type: GLTF_COMPONENT_TYPE {}; Name {}", positionsType, meshName);
			meshes.pop_back();
			return Handle<Mesh>::sNone;
		}
	} else {
		SPDLOG_ERROR("Mesh positions not loaded, not supported type: GLTF_TYPE {}; Name {}", positionsTypeCount, meshName);
		meshes.pop_back();
		return Handle<Mesh>::sNone;
	}


	// Load normals
	const tinygltf::Accessor& normalsAccessor = gltfModel.accessors[primitive.attributes["NORMAL"]];
	Int32 normalsType = normalsAccessor.componentType;
	Int32 normalsTypeCount = normalsAccessor.type;

	if (normalsTypeCount == TINYGLTF_TYPE_VEC3)
	{
		if (normalsType == TINYGLTF_COMPONENT_TYPE_FLOAT)
		{
			process_accessor<glm::vec3>(gltfModel, normalsAccessor, mesh.normals);
		} else {
			SPDLOG_ERROR("Mesh normals not loaded, not supported type: GLTF_COMPONENT_TYPE {}; Name {}", normalsType, meshName);
			meshes.pop_back();
			return Handle<Mesh>::sNone;
		}
	} else {
		SPDLOG_ERROR("Mesh normals not loaded, not supported type: GLTF_TYPE {}; Name {}", normalsTypeCount, meshName);
		meshes.pop_back();
		return Handle<Mesh>::sNone;
	}


	// Load uvs
	const tinygltf::Accessor& uvsAccessor = gltfModel.accessors[primitive.attributes["TEXCOORD_0"]];
	Int32 uvsType						  = uvsAccessor.componentType;
	Int32 uvsTypeCount					  = uvsAccessor.type;

	if (uvsTypeCount == TINYGLTF_TYPE_VEC2)
	{
		if (uvsType == TINYGLTF_COMPONENT_TYPE_FLOAT)
		{
			process_accessor<glm::vec2>(gltfModel, uvsAccessor, mesh.uvs);
		} else {
			SPDLOG_ERROR("Mesh uvs not loaded, not supported type: GLTF_COMPONENT_TYPE {}; Name {}", uvsType, meshName);
			meshes.pop_back();
			return Handle<Mesh>::sNone;
		}
	} else {
		SPDLOG_ERROR("Mesh uvs not loaded, not supported type: GLTF_TYPE {}; Name {}", uvsTypeCount, meshName);
		meshes.pop_back();
		return Handle<Mesh>::sNone;
	}

	Handle<Mesh> meshHandle{ Int32(meshId) };
	nameToIdMeshes[meshName] = meshHandle;

	return meshHandle;
}

Handle<Material> SResourceManager::load_material(const std::filesystem::path& assetPath, tinygltf::Material& gltfMaterial, tinygltf::Model& gltfModel)
{
	if (nameToIdMaterials.find(gltfMaterial.name) != nameToIdMaterials.end())
	{
		SPDLOG_ERROR("Material with name {} already exist!", gltfMaterial.name);
		return Handle<Material>::sNone;
	}

	materials.emplace_back();
	Int64 materialId   = materials.size() - 1;
	Material& material = materials[materialId];

	const Int32 albedoId			  = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
	const Int32 metallicRoughnessId	  = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
	const Int32 normalId			  = gltfMaterial.normalTexture.index;
	const Int32 ambientOcclusionId    = gltfMaterial.occlusionTexture.index;
	const Int32 emissionId		      = gltfMaterial.emissiveTexture.index;

	if (albedoId >= 0)
	{
		const tinygltf::Texture& texture = gltfModel.textures[albedoId];
		const tinygltf::Image& image = gltfModel.images[texture.source];
		const std::filesystem::path textureName = image.uri;
		material.albedo = load_texture(assetPath / image.uri, textureName.stem().string(), ETextureType::Albedo);
	}

	if (metallicRoughnessId >= 0)
	{
		const tinygltf::Texture& texture = gltfModel.textures[metallicRoughnessId];
		const tinygltf::Image& image = gltfModel.images[texture.source];
		const std::filesystem::path textureName = image.uri;
		material.metalness = load_texture(assetPath / image.uri, textureName.stem().string(), ETextureType::RM);
		material.roughness = material.metalness;
	}

	if (normalId >= 0)
	{
		const tinygltf::Texture& texture = gltfModel.textures[normalId];
		const tinygltf::Image& image = gltfModel.images[texture.source];
		const std::filesystem::path textureName = image.uri;
		material.normal = load_texture(assetPath / image.uri, textureName.stem().string(), ETextureType::Normal);
	}

	if (ambientOcclusionId >= 0)
	{
		const tinygltf::Texture& texture = gltfModel.textures[ambientOcclusionId];
		const tinygltf::Image& image = gltfModel.images[texture.source];
		const std::filesystem::path textureName = image.uri;
		material.ambientOcclusion = load_texture(assetPath / image.uri, textureName.stem().string(), ETextureType::AmbientOcclusion);
	}

	if (emissionId >= 0)
	{
		const tinygltf::Texture& texture = gltfModel.textures[emissionId];
		const tinygltf::Image& image = gltfModel.images[texture.source];
		const std::filesystem::path textureName = image.uri;
		material.emission = load_texture(assetPath / image.uri, textureName.stem().string(), ETextureType::Emission);
	}

	Handle<Material> materialHandle{ Int32(materialId) };
	nameToIdMaterials[gltfMaterial.name] = materialHandle;

	return materialHandle;
}

Handle<Texture> SResourceManager::load_texture(const std::filesystem::path& filePath, const std::string& textureName, ETextureType type)
{
	if (nameToIdTextures.find(textureName) != nameToIdTextures.end())
	{
		SPDLOG_ERROR("Texture with name {} already exist!", textureName);
		return Handle<Texture>::sNone;
	}

	textures.emplace_back();
	Int64 textureId = textures.size() - 1;
	Texture& texture = textures[textureId];

	texture.data = stbi_load(filePath.string().c_str(), &texture.size.x, &texture.size.y, &texture.channels, 0);
	texture.type = type;
	if (!texture.data)
	{
		SPDLOG_ERROR("Texture {} loading failed.", filePath.string());
		textures.pop_back();
		return Handle<Texture>::sNone;
	}

	Handle<Texture> textureHandle{ Int32(textureId) };
	nameToIdTextures[textureName] = textureHandle;

	return textureHandle;
}

Handle<Material> SResourceManager::create_material(Material& material, const std::string& name)
{
	if (nameToIdMaterials.contains(name))
	{
		SPDLOG_ERROR("Material with name {} is already exist.", name);
		return Handle<Material>::sNone;
	}
	materials.emplace_back(material);
	Handle<Material> materialHandle{ Int32(materials.size()) - 1 };
	nameToIdMaterials[name] = materialHandle;

	return materialHandle;
}

Handle<Model> SResourceManager::create_model(const Model &model, const std::string& name)
{
	if (nameToIdModels.contains(name))
	{
		SPDLOG_ERROR("Model with name {} is already exist.", name);
		return Handle<Model>::sNone;
	}
	models.emplace_back(model);
	Handle<Model> modelHandle{ Int32(models.size()) - 1 };
	nameToIdModels[name] = modelHandle;

	return modelHandle;
}

Handle<Mesh> SResourceManager::create_mesh(const std::string& name)
{
	if (nameToIdMeshes.contains(name))
	{
		SPDLOG_ERROR("Mesh with name {} is already exist.", name);
		return Handle<Mesh>::sNone;
	}
	meshes.emplace_back();
	Handle<Mesh> meshHandle{ Int32(meshes.size()) - 1 };
	nameToIdMeshes[name] = meshHandle;

	return meshHandle;
}

Model& SResourceManager::get_model_by_name(const std::string& name)
{
	const auto& iterator = nameToIdModels.find(name);
	if (iterator == nameToIdModels.end() || iterator->second.id < 0 || iterator->second.id >= models.size())
	{
		SPDLOG_WARN("Model {} not found, returned default.", name);
		return models[0];
	}

	return models[iterator->second.id];
}

Model& SResourceManager::get_model_by_handle(const Handle<Model> handle)
{
	if (handle.id >= models.size())
	{
		SPDLOG_WARN("Model {} not found, returned default.", handle.id);
		return models[0];
	}
	return models[handle.id];
}

Mesh& SResourceManager::get_mesh_by_name(const std::string& name)
{
	const auto& iterator = nameToIdMeshes.find(name);
	if (iterator == nameToIdMeshes.end() || iterator->second.id < 0 || iterator->second.id >= meshes.size())
	{
		SPDLOG_WARN("Mesh {} not found, returned default.", name);
		return meshes[0];
	}

	return meshes[iterator->second.id];
}

Mesh& SResourceManager::get_mesh_by_handle(const Handle<Mesh> handle)
{
	if (handle.id >= meshes.size())
	{
		SPDLOG_WARN("Mesh {} not found, returned default.", handle.id);
		return meshes[0];
	}
	return meshes[handle.id];
}

Material& SResourceManager::get_material_by_name(const std::string& name)
{
	const auto& iterator = nameToIdMaterials.find(name);
	if (iterator == nameToIdMaterials.end() || iterator->second.id < 0 || iterator->second.id >= materials.size())
	{
		SPDLOG_WARN("Material {} not found, returned default.", name);
		return materials[0];
	}

	return materials[iterator->second.id];
}

Material& SResourceManager::get_material_by_handle(const Handle<Material> handle)
{
	if (handle.id >= materials.size())
	{
		SPDLOG_WARN("Material {} not found, returned default.", handle.id);
		return materials[0];
	}
	return materials[handle.id];
}

Material& SResourceManager::get_default_material()
{
	return get_material_by_name("DefaultMaterial");
}

Texture& SResourceManager::get_texture_by_name(const std::string& name)
{
	const auto& iterator = nameToIdTextures.find(name);
	if (iterator == nameToIdTextures.end() || iterator->second.id < 0 || iterator->second.id >= textures.size())
	{
		SPDLOG_WARN("Texture {} not found, returned default.", name);
		return textures[0];
	}

	return textures[iterator->second.id];
}

Texture& SResourceManager::get_texture_by_handle(const Handle<Texture> handle)
{
	if (handle.id >= textures.size())
	{
		SPDLOG_WARN("Texture {} not found, returned default.", handle.id);
		return textures[0];
	}
	return textures[handle.id];
}

const Handle<Model> &SResourceManager::get_model_handle_by_name(const std::string &name)
{
	const auto &iterator = nameToIdModels.find(name);
	if (iterator == nameToIdModels.end())
	{
		SPDLOG_WARN("Model handle {} not found, returned none.", name);
		return Handle<Model>::sNone;
	}
	return iterator->second;
}

const Handle<Mesh> &SResourceManager::get_mesh_handle_by_name(const std::string &name)
{
	const auto &iterator = nameToIdMeshes.find(name);
	if (iterator == nameToIdMeshes.end())
	{
		SPDLOG_WARN("Mesh handle {} not found, returned none.", name);
		return Handle<Mesh>::sNone;
	}
	return iterator->second;
}

const Handle<Material> &SResourceManager::get_material_handle_by_name(const std::string &name)
{
	const auto &iterator = nameToIdMaterials.find(name);
	if (iterator == nameToIdMaterials.end())
	{
		SPDLOG_WARN("Material handle {} not found, returned none.", name);
		return Handle<Material>::sNone;
	}
	return iterator->second;
}

const Handle<Texture>& SResourceManager::get_texture_handle_by_name(const std::string& name)
{
	const auto &iterator = nameToIdTextures.find(name);
	if (iterator == nameToIdTextures.end())
	{
		SPDLOG_WARN("Texture handle {} not found, returned none.", name);
		return Handle<Texture>::sNone;
	}
	return iterator->second;
}

const std::vector<Model>& SResourceManager::get_models() const
{
	return models;
}

const std::vector<Mesh>& SResourceManager::get_meshes() const
{
	return meshes;
}

const std::vector<Material>& SResourceManager::get_materials() const
{
	return materials;
}

const std::vector<Texture>& SResourceManager::get_textures() const
{
	return textures;
}

void SResourceManager::shutdown()
{
	SPDLOG_INFO("Resource Manager shutdown.");
	nameToIdTextures.clear();
	for (Texture& texture : textures)
	{
		if (texture.gpuId)
		{
			glDeleteTextures(1, &texture.gpuId);
			texture.gpuId = 0;
		}
		stbi_image_free(texture.data);
		texture.type = ETextureType::None;
	}
	textures.clear();

	nameToIdMaterials.clear();
	materials.clear();

	nameToIdMeshes.clear();
	for (Mesh& mesh : meshes)
	{
		if (mesh.gpuIds[0])
		{
			glDeleteVertexArrays(1, &mesh.gpuIds[0]);
			mesh.gpuIds[0] = 0;
			glDeleteBuffers(2, &mesh.gpuIds[1]);
			mesh.gpuIds[1] = 0;
			mesh.gpuIds[2] = 0;
		}
	}
	meshes.clear();

	nameToIdModels.clear();
	models.clear();
}
