#pragma once

// An enumeration of all possible bindings to be added
// to Descriptor Sets. Primarily only uniform buffers
// and texture samplers will be used to pass information
// to the vertex and fragment shaders respectively
// but other advanced options are available.
enum BindingType
{
	// A uniform buffer, to be defined with a type by an object of the UniformType enum
	UNIFORM_BUFFER = 0,
	// A texture sampler, to be pulled from the Boturi environment with Boturi::getTextureSampler(int mipLevel)
	TEXTURE_SAMPLER = 1
};