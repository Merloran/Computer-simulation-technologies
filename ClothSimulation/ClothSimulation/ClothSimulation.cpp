#include <GLFW/glfw3.h>

#include "source/input_manager.hpp"
#include "source/simulation_manager.hpp"
#include "source/display_manager.hpp"
#include "source/resource_manager.hpp"
#include "source/render_manager.hpp"
#include "source/Common/camera.hpp"

void input_setup();
void handle_camera(Camera& cam, float dt);

int main()
{
	SDisplayManager &displayManager = SDisplayManager::get();
	SResourceManager &resourceManager = SResourceManager::get();
	SRenderManager &renderManager = SRenderManager::get();
	InputManager& inputManager = InputManager::get();
	SimulationManager &simulationManager = SimulationManager::get();

	displayManager.startup();
	resourceManager.startup();
	inputManager.startup();
	renderManager.startup();
	simulationManager.startup();
	input_setup();

	Camera camera;
	camera.initialize({ -20.0f, 20.0f, 25.0f });
	bool inCameraMode = false;

	Float32 lastFrame = 0.0f;
	Float32 deltaTimeMs = 0.0f;
	while(!displayManager.should_window_close())
	{
		Float32 currentFrame = glfwGetTime();
		deltaTimeMs = currentFrame - lastFrame;
		lastFrame = currentFrame;


		//handle camera
		if (inputManager.is_action_just_pressed("toggle_camera_mode")) {
			inCameraMode = !inCameraMode;
			displayManager.capture_mouse(inCameraMode);
		}
		if (inCameraMode) {
			handle_camera(camera, deltaTimeMs);
		}

		inputManager.process_input();
		displayManager.update();
		simulationManager.update();
		renderManager.update(camera);
	}

	simulationManager.shutdown();
	inputManager.shutdown();
	renderManager.shutdown();
	resourceManager.shutdown();
	displayManager.shutdown();
}

void input_setup() {
	InputManager& input_manager = InputManager::get();
	input_manager.add_action("move_forward");
	input_manager.add_key_to_action("move_forward", InputKey::W);
	input_manager.add_action("move_backward");
	input_manager.add_key_to_action("move_backward", InputKey::S);
	input_manager.add_action("move_left");
	input_manager.add_key_to_action("move_left", InputKey::A);
	input_manager.add_action("move_right");
	input_manager.add_key_to_action("move_right", InputKey::D);
	input_manager.add_action("move_up");
	input_manager.add_key_to_action("move_up", InputKey::SPACE);
	input_manager.add_action("move_down");
	input_manager.add_key_to_action("move_down", InputKey::LEFT_SHIFT);
	input_manager.add_action("move_faster");
	input_manager.add_key_to_action("move_faster", InputKey::LEFT_CONTROL);

	input_manager.add_action("toggle_camera_mode");
	input_manager.add_key_to_action("toggle_camera_mode", InputKey::ESCAPE);
}


void handle_camera(Camera& cam, float dt) {
	InputManager& input_manager = InputManager::get();
	float forward = input_manager.get_axis("move_backward", "move_forward");
	float right = input_manager.get_axis("move_left", "move_right");
	float up = input_manager.get_axis("move_down", "move_up");

	if (input_manager.is_action_pressed("move_faster")) {
		dt *= 3.0f;
	}

	cam.move_forward(forward * dt);
	cam.move_right(right * dt);
	cam.move_up(up * dt);

	glm::vec2 mouse_delta = input_manager.get_mouse_delta();
	cam.rotate(mouse_delta.x * dt, mouse_delta.y * dt);
}