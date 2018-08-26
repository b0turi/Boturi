#pragma once

class Texture : Image
{
private:
	uint32_t mipLevels;
public:
	Texture();
	Texture(const char * filename);

	void cleanup();
};