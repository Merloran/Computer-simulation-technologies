#include "source/display_manager.hpp"
#include "source/resource_manager.hpp"
#include "source/render_manager.hpp"

int main()
{
	SDisplayManager &displayManager = SDisplayManager::get();
	SResourceManager &resourceManager = SResourceManager::get();
	SRenderManager &renderManager = SRenderManager::get();

	displayManager.startup();
	resourceManager.startup();
	renderManager.startup();

	while(!displayManager.should_window_close())
	{
		displayManager.update();
	}

	renderManager.shutdown();
	resourceManager.shutdown();
	displayManager.shutdown();
}