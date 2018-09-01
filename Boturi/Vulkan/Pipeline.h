#pragma once
struct PipelineSettings
{
	std::vector<VkViewport> viewports = getBlankViewport();
	std::vector<VkRect2D> scissors = getBlankScissor();
	bool depthClamp = false;
	VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
	float lineWidth = 1.0f;
	VkCullModeFlagBits culling = VK_CULL_MODE_BACK_BIT;
	VkFrontFace front = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	int msaaSamples;

private:
	std::vector<VkViewport> getBlankViewport()
	{
		VkViewport viewport = {};
		return { viewport };
	}

	std::vector<VkRect2D> getBlankScissor()
	{
		VkRect2D scissor = {};
		return { scissor };
	}
};
class Pipeline
{
private:
	VkPipelineLayout layout;
	VkPipeline pipeline;

	VkDescriptorSetLayout descriptorLayout;
public:
	Shader shader;

	Pipeline();
	Pipeline(Shader shader, VkDescriptorSetLayout layout);

	void cleanup();

	VkPipeline getPipeline();
	VkPipelineLayout getLayout();
};