#include "render_manager.hpp"

#include "display_manager.hpp"
#include "resource_manager.hpp"
#include "Common/model.hpp"
#include "Common/mesh.hpp"
#include "Common/texture.hpp"
#include "Common/material.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

SRenderManager& SRenderManager::get()
{
	static SRenderManager instance;
	return instance;
}

void SRenderManager::startup()
{
	SPDLOG_INFO("Render Manager startup.");
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		SPDLOG_ERROR("Failed to initialize OpenGL loader!");
		SDisplayManager::get().close_window();
		return;
	}
}

void SRenderManager::draw_model(const Model& model)
{
	
	SResourceManager& resourceManager = SResourceManager::get();

    for (Int32 i = 0; i < model.meshes.size(); i++)
    {
		Mesh& mesh = resourceManager.get_mesh_by_handle(model.meshes[i]);
		Material& material = resourceManager.get_material_by_handle(model.materials[i]);

		Handle<Texture>* textureHandle = &material.albedo;
		for (Int32 j = 0; j < sizeof(Material) / sizeof(Handle<Texture>); ++j, ++textureHandle)
		{
			if (*textureHandle != Handle<Texture>::sNone)
			{
				//SPDLOG_INFO(magic_enum::enum_name(resourceManager.get_texture_by_handle(*textureHandle).type));
			}
		}
		

		auto& [VAO, EBO] = mesh.gpuIds;
		glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, mesh.indexes.size(), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    
}

void SRenderManager::shutdown()
{
	SPDLOG_INFO("Render Manager shutdown.");
}
