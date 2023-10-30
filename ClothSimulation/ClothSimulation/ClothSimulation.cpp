#include <GLFW/glfw3.h>

#include "source/simulation_manager.hpp"
#include "source/display_manager.hpp"
#include "source/resource_manager.hpp"
#include "source/render_manager.hpp"
#include "source/Common/camera.hpp"
#include "source/Common/model.hpp"
#include "source/Common/handle.hpp"
#include "source/Common/mesh.hpp"

int main()
{
	SDisplayManager &displayManager = SDisplayManager::get();
	SResourceManager &resourceManager = SResourceManager::get();
	SRenderManager &renderManager = SRenderManager::get();
	SimulationManager &simulationManager = SimulationManager::get();

	displayManager.startup();
	resourceManager.startup();
	renderManager.startup();
	simulationManager.startup();

	Camera camera;
	camera.initialize({ 20.0f, 20.0f, 40.0f });
	Float32 lastFrame = 0.0f;
	Float32 deltaTimeMs = 0.0f;
	while(!displayManager.should_window_close())
	{
		Float32 currentFrame = glfwGetTime();
		deltaTimeMs = currentFrame - lastFrame;
		lastFrame = currentFrame;

		displayManager.update();
		simulationManager.update(deltaTimeMs * 0.1f);
		renderManager.update(camera, deltaTimeMs);
	}

	simulationManager.shutdown();
	renderManager.shutdown();
	resourceManager.shutdown();
	displayManager.shutdown();
}