#pragma once

class SDisplayManager
{
public:
	SDisplayManager(SDisplayManager&) = delete;
	static SDisplayManager& get();

	void startup();

	const glm::ivec2& get_framebuffer_size();
	const glm::ivec2& get_window_size();
	Float32 get_aspect_ratio() const;

	void update();
	void close_window();
	bool should_window_close();
	struct GLFWwindow& get_window() const;

	void capture_mouse(bool capture) const;

	void shutdown();

private:
	SDisplayManager() = default;
	~SDisplayManager() = default;

	std::string name		  = "BaseWindow";
	struct GLFWwindow* window = nullptr;
	glm::ivec2 windowSize	  = { 1024, 768 };
	glm::ivec2 framebufferSize;
};

