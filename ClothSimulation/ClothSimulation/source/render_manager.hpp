#pragma once
#include "Common/shader.hpp"

class SRenderManager
{
public:
	SRenderManager(SRenderManager&) = delete;
	static SRenderManager& get();

	void startup();

	void update(class Camera& camera, Float32 dT);

	void draw_model(const struct Model& model);
	void draw_sphere(const glm::vec3 &color);
	void add_line(const glm::vec3 &begin, const glm::vec3 &end);
	void draw_lines(const glm::vec3 &color);

	void shutdown();

private:
	SRenderManager() = default;
	~SRenderManager() = default;

	void camera_gui(class Camera& camera);
	Shader diffuse, normals;
	std::vector<glm::vec3> positions;
};

