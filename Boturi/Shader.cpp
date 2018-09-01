#include "Boturi.h"
#include <Windows.h>
#include <WinBase.h>

Shader::Shader() {}

Shader::Shader(std::vector<std::string> otherFiles, 
	std::vector<BindingType> definition, 
	std::vector<UniformType> uniformTypes,
	bool defaultPipeline)
{
	// Only used during development, switch to full spir-v after shaders are finalized

	// Go into the shaders directory and convert the shaders to spir-v files
	SetCurrentDirectory("shaders");

	for (auto file : otherFiles)
	{
		std::string shaderName = file.substr(0, file.find('.'));
		std::string cmd = "glslangValidator.exe -V " + file + " -o " + shaderName + ".spv";
		system(cmd.c_str());

		filenames.push_back("shaders/" + shaderName + ".spv");

		Shader::name = shaderName.substr(0, shaderName.size() - 2);
	}
	
	SetCurrentDirectory("..");
	
	Shader::definition = definition;
	Shader::uniformTypes = uniformTypes;
	Shader::defaultPipeline = defaultPipeline;
}

std::vector<std::string> Shader::getFilenames() { return filenames; }
std::vector<BindingType> Shader::getDefinition() {
	return definition; 
}
std::vector<UniformType> Shader::getUniformTypes() { return uniformTypes; }
std::string Shader::getName() { return name; }