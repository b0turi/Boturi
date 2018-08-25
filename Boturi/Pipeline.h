#pragma once

class Pipeline
{
private:
	VkPipelineLayout layout;
	VkPipeline pipeline;
public:
	Pipeline();
	Pipeline(const char * vertexShader, const char * fragmentShader, Descriptor desc);

	void cleanup();
};