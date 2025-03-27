#pragma once
#include <unordered_map>
#include "../components/light_component.h"
#include "../components/transform_component.h"
class LightSystem
{
public : 
	LightSystem(unsigned int shader, unsigned int reflectionShader);
	void update(std::unordered_map<unsigned int, LightComponent>&, std::unordered_map<unsigned int, TransformComponent>&, unsigned int);

private:
	unsigned int shaderProgram;
	unsigned int reflectionShaderProgram;

};

