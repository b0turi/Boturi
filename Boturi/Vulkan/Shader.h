#pragma once
#include <string>
#include <vector>
class Shader
{
private:
	std::vector<BindingType> definition;
	std::vector<UniformType> uniformTypes;
	bool defaultPipeline = true;
	std::vector<std::string> filenames;

	std::string name;
public:
	Shader();
	Shader(std::vector<std::string> filenames,
		std::vector<BindingType> definition,
		std::vector<UniformType> uniformTypes,
		bool defaultPipeline);

	std::vector<std::string> getFilenames();
	std::vector<BindingType> getDefinition();
	std::vector<UniformType> getUniformTypes();
	std::string getName();
};