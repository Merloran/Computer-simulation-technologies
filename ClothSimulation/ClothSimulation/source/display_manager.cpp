#include "display_manager.hpp"

#include <GLFW/glfw3.h>


SDisplayManager& SDisplayManager::get()
{
	static SDisplayManager instance;
	return instance;
}

void SDisplayManager::startup()
{
	SPDLOG_INFO("Display Manager startup.");
	if (!glfwInit())
	{
		SPDLOG_ERROR("Failed to initialize GLFW!");
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(windowSize.x, windowSize.y, name.c_str(), nullptr, nullptr);

	if (!window)
	{
		SPDLOG_ERROR("Failed create window!");
		return;
	}
	glfwMakeContextCurrent(window);
}

const glm::ivec2& SDisplayManager::get_framebuffer_size()
{
	if (window == nullptr)
	{
		SPDLOG_ERROR("Window is null!");
		return { -1, -1 };
	}
	glfwGetFramebufferSize(window, &framebufferSize.x, &framebufferSize.y);
	return framebufferSize;
}

const glm::ivec2& SDisplayManager::get_window_size()
{
	if (window == nullptr)
	{
		SPDLOG_ERROR("Window is null!");
		return { -1, -1 };
	}
	glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
	return windowSize;
}

Float32 SDisplayManager::get_aspect_ratio() const
{
	if (windowSize.y == 0)
	{
		return 1.0f;
	}

	return Float32(windowSize.x) / Float32(windowSize.y);
}

void SDisplayManager::update()
{
	glfwPollEvents();
	glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
	glfwGetFramebufferSize(window, &framebufferSize.x, &framebufferSize.y);
	glViewport(0, 0, windowSize.x, windowSize.y);
	glClearColor(0.31f, 0.22f, 0.16f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDisplayManager::close_window()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

bool SDisplayManager::should_window_close()
{
	if (window == nullptr)
	{
		SPDLOG_ERROR("Window is null!");
		return true;
	}

	return glfwWindowShouldClose(window);
}

GLFWwindow& SDisplayManager::get_window() const
{
	return *window;
}

void SDisplayManager::capture_mouse(bool capture) const
{
	glfwSetInputMode(window, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void SDisplayManager::shutdown()
{
	SPDLOG_INFO("Display Manager shutdown.");
	glfwDestroyWindow(window);
	glfwTerminate();
	window = nullptr;
}
