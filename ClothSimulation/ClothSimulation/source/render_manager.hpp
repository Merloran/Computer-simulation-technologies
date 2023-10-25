#pragma once
class SRenderManager
{
public:
	SRenderManager(SRenderManager&) = delete;
	static SRenderManager& get();

	void startup();
	void draw_model(const struct Model& model);
	void shutdown();

private:
	SRenderManager() = default;
	~SRenderManager() = default;
};

