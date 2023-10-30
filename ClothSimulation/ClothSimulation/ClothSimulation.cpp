#include <GLFW/glfw3.h>

#include "source/display_manager.hpp"
#include "source/resource_manager.hpp"
#include "source/render_manager.hpp"
#include "source/Common/camera.hpp"
#include "source/Common/shader.hpp"
#include "source/Common/mesh.hpp"
#include "source/Common/model.hpp"
#include "source/Common/handle.hpp"

int main()
{
	SDisplayManager &displayManager = SDisplayManager::get();
	SResourceManager &resourceManager = SResourceManager::get();
	SRenderManager &renderManager = SRenderManager::get();

	displayManager.startup();
	resourceManager.startup();
	renderManager.startup();


	resourceManager.load_gltf_asset("Resources/Assets/Flag/Flag.gltf");
	std::vector<Model> models = resourceManager.get_models();
	if (!models.empty())
	{
		resourceManager.generate_opengl_model(models[0]);
	}

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
		renderManager.update(camera, deltaTimeMs);
	}

	renderManager.shutdown();
	resourceManager.shutdown();
	displayManager.shutdown();
}