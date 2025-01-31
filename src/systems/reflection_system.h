#pragma once
#include <unordered_map>
#include "../components/render_component.h"
#include "../components/transform_component.h"
class ReflectionSystem
{
private:
	unsigned int baseShader;
	unsigned int reflectionShader;

	unsigned int reflectionHeight = 0;
	unsigned int reflectionWidth = 0;

public:
	ReflectionSystem(unsigned int baseShader, unsigned int reflectionShader);
	void Initialize(std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents, int reflectionWidth, int reflectionHeight);
	void RenderReflection(
		std::unordered_map<unsigned int, TransformComponent>& transformComponents,
		std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents,
		glm::mat4 viewMatrix, glm::mat4 projectionMatrix, int screenWidth, int screenHeight, int cameraID);
};

