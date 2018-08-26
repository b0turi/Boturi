#pragma once

class Texture : public Image
{
private:
	uint32_t mipLevels;
public:
	static VkSampler makeTextureSampler(int mipLevel);
	Texture();
	Texture(const char * filename);
	int getMipLevels();
};