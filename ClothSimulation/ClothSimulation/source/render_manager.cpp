#include "render_manager.hpp"

#include "display_manager.hpp"
#include "resource_manager.hpp"
#include "Common/model.hpp"
#include "Common/mesh.hpp"
#include "Common/texture.hpp"
#include "Common/material.hpp"
#include "Common/shader.hpp"
#include "Common/camera.hpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "simulation_manager.hpp"

SRenderManager& SRenderManager::get()
{
	static SRenderManager instance = SRenderManager();
	return instance;
}

void SRenderManager::startup()
{
	SPDLOG_INFO("Render Manager startup.");
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		SPDLOG_ERROR("Failed to initialize OpenGL loader!");
		SDisplayManager::get().close_window();
		return;
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	const SDisplayManager &displayManager = SDisplayManager::get();

	ImGui_ImplGlfw_InitForOpenGL(&displayManager.get_window(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	// Setup style
	ImGui::StyleColorsDark();
	ImGuiStyle &style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	diffuse.create("Resources/Shaders/Vertex.vert",
				  "Resources/Shaders/Fragment.frag");

	normals.create("Resources/Shaders/Normals.vert",
				  "Resources/Shaders/Normals.frag",
				  "Resources/Shaders/Normals.geom");
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void SRenderManager::update(Camera &camera, Float32 dT)
{
	const SDisplayManager &displayManager = SDisplayManager::get();
	SimulationManager &simulationManager = SimulationManager::get();

	// IMGUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	camera_gui(camera);
	simulationManager.show_gui();

	ImGui::Render();

	glfwMakeContextCurrent(&displayManager.get_window());
	
	diffuse.use();
	glm::mat4 view = camera.get_view();
	glm::mat4 proj = camera.get_projection(displayManager.get_aspect_ratio());
	glm::mat4 model = glm::mat4(1.0f);
	diffuse.set_mat4("projection", proj);
	diffuse.set_mat4("view", view);
	const glm::vec3  origin   = { 10.0f , 30.0f, -5.0f };
	const std::vector<MassPoint>& massPoints = simulationManager.get_mass_points();
	const std::vector<Spring>& springs = simulationManager.get_springs();

	for (const MassPoint& massPoint : massPoints)
	{
		model = glm::translate(glm::mat4(1.0f),origin + massPoint.position);
		model = glm::scale(model, glm::vec3(simulationManager.get_initial_length() * 0.25f));
		diffuse.set_mat4("model", model);
		draw_sphere(glm::vec3(1.0f));
	}
	
	for (const Spring& spring : springs)
	{
		add_line(massPoints[spring.indexA].position,
				 massPoints[spring.indexB].position);
	}

	diffuse.set_mat4("model", glm::translate(glm::mat4(1.0f), origin));
	draw_lines(glm::vec3(1.0f));
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(&displayManager.get_window());
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
		
		
		glBindVertexArray(mesh.gpuIds[0]);
        glDrawElements(GL_TRIANGLES, mesh.indexes.size(), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    
}

void SRenderManager::draw_sphere(const glm::vec3& color)
{
	static UInt32 sphereVAO = 0;
	static UInt32 indexCount = 0;
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		UInt32 VBO, EBO;
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<UInt32> indexes;

		const Int32 xSegments = 32, ySegments = 32;
		const Float32 radius = 1.0f;
		// vertex position
		Float32 lengthInv = 1.0f / radius;    // vertex normal
		// vertex texCoord

		const Float32 xStep = glm::two_pi<Float32>() / Float32(xSegments);
		const Float32 yStep = glm::pi<Float32>() / Float32(ySegments);
		Float32 xAngle = 0.0f, yAngle = 0.0f;

		for (Int32 i = 0; i <= ySegments; ++i)
		{
			yAngle = glm::pi<Float32>() / 2.0f - Float32(i) * yStep;        // starting from pi/2 to -pi/2
			Float32 xy = radius * cosf(yAngle);             // r * cos(u)
			Float32 z = radius * sinf(yAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// first and last vertices have same position and normal, but different tex coords
			for (Int32 j = 0; j <= xSegments; ++j)
			{
				xAngle = j * xStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				Float32 x = xy * cosf(xAngle);             // r * cos(u) * cos(v)
				Float32 y = xy * sinf(xAngle);             // r * cos(u) * sin(v)
				positions.emplace_back(x, y, z);

				// normalized vertex normal (nx, ny, nz)
				normals.emplace_back(glm::normalize(glm::vec3(x, y, z)));

				// vertex tex coord (s, t) range between [0, 1]
				Float32 s = (Float32)j / xSegments;
				Float32 t = (Float32)i / ySegments;
				uvs.emplace_back(s, t);
			}
		}
		
		UInt32 k1, k2;
		for (Int32 i = 0; i < ySegments; ++i)
		{
			k1 = i * (xSegments + 1);     // beginning of current stack
			k2 = k1 + xSegments + 1;      // beginning of next stack

			for (Int32 j = 0; j < xSegments; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indexes.push_back(k1);
					indexes.push_back(k2);
					indexes.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (ySegments - 1))
				{
					indexes.push_back(k1 + 1);
					indexes.push_back(k2);
					indexes.push_back(k2 + 1);
				}
			}
		}

		indexCount = indexes.size();

		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		const Int64 positionsSize = positions.size() * sizeof(glm::vec3);
		const Int64 normalsSize = normals.size() * sizeof(glm::vec3);
		const Int64 uvsSize = uvs.size() * sizeof(glm::vec2);

		glBufferData(GL_ARRAY_BUFFER, positionsSize + normalsSize + uvsSize, nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positionsSize, positions.data());
		glBufferSubData(GL_ARRAY_BUFFER, positionsSize, normalsSize, normals.data());
		glBufferSubData(GL_ARRAY_BUFFER, positionsSize + normalsSize, uvsSize, uvs.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(UInt32), indexes.data(), GL_STATIC_DRAW);

		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
		// Normal attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)positionsSize);
		// Texture position attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)(positionsSize + normalsSize));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	diffuse.set_vec3("objectColor", color);
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void SRenderManager::add_line(const glm::vec3& begin, const glm::vec3& end)
{
	positions.push_back(begin);
	positions.push_back(end);
}

void SRenderManager::draw_lines(const glm::vec3& color)
{
	static UInt32 linesVAO = 0, linesVBO = 0;
	if (linesVAO == 0)
	{
		glGenVertexArrays(1, &linesVAO);
		glGenBuffers(1, &linesVBO);

		glBindVertexArray(linesVAO);
		glBindBuffer(GL_ARRAY_BUFFER, linesVBO);

		constexpr UInt32 MAX_VERTEX_COUNT = 1000000;
		glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
		
		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (positions.empty())
	{
		return;
	}
	diffuse.set_vec3("objectColor", color);

	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());
	
	glLineWidth(10.0f);
	glDisable(GL_LINE_SMOOTH);

	glBindVertexArray(linesVAO);
	glDrawArrays(GL_LINES, 0, positions.size());
	glBindVertexArray(0);
	positions.clear();
}

void SRenderManager::shutdown()
{
	SPDLOG_INFO("Render Manager shutdown.");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	diffuse.shutdown();
	normals.shutdown();
}

void SRenderManager::camera_gui(Camera &camera)
{
	ImGui::Begin("Camera settings");
	
	ImGui::DragFloat3("Position", &camera.position[0], 0.1f);
	ImGui::SliderFloat("Pitch", &camera.pitch, -89.0f, 89.0f);
	ImGui::SliderFloat("Yaw", &camera.yaw, -180.0f, 180.0f);

	ImGui::End();
	camera.update_camera_vectors();
}
