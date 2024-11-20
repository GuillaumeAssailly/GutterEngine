#pragma once
#include <array>
#include "../config.h"
#include "../components/light_component.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"
#include "../components/camera_component.h"


#define SHADOW_WIDTH 4096*2
#define SHADOW_HEIGHT 4096*2



class ShadowSystem
{

public:
	ShadowSystem(unsigned int, unsigned int, unsigned int);
	~ShadowSystem();
	void Initialize(std::unordered_map<unsigned int, LightComponent>& lightComponents);
	void GenerateShadowMap(std::unordered_map<unsigned int, LightComponent>& lightComponents,
		std::unordered_map<unsigned int, TransformComponent>& transformComponents,
		std::unordered_map<unsigned int, RenderComponent>& renderComponents, int screenWidth, int screenHeight,
		CameraComponent&);

	void CalculateShadowOrthoBounds(const glm::mat4& lightView,
		CameraComponent& camera,
		float& left, float& right, float& bottom, float& top,
		float& near_plane, float& far_plane);

	void RenderDepthMap(unsigned int depthMap, int, int);
private:
	unsigned int shader;
	unsigned int shadowShader;
	unsigned int depthMapShader;

	unsigned int quadVAO, quadVBO;

	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

	


};

