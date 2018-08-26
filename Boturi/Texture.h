#pragma once

class Texture : Image
{
private:
	uint32_t mipLevels;
public:
	static VkSampler makeTextureSampler(int mipLevel);
	Texture();
	Texture(const char * filename);

	void cleanup();
};