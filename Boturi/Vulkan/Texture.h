#pragma once

class Texture : public Image
{
private:
	uint32_t mipLevels;
public:
	static VkSampler makeTextureSampler(int mipLevel);
	static unsigned char * loadImage(const char * filename, int * width, int * height);
	Texture();
	Texture(const char * filename);
	int getMipLevels();
};