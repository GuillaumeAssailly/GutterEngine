#pragma once
#include <array>
#include "../config.h"
#include "../components/light_component.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"
#include "../components/camera_component.h"


#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096



class ShadowSystem
{

public:
	ShadowSystem(unsigned int, unsigned int, unsigned int);
	~ShadowSystem();
	void Initialize(std::unordered_map<unsigned int, LightComponent>& lightComponents);
	void GenerateShadowMap(std::unordered_map<unsigned int, LightComponent>& lightComponents,
		std::unordered_map<unsigned int, TransformComponent>& transformComponents,
		std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents, int screenWidth, int screenHeight,
		int cameraID);

	void CalculateShadowOrthoBounds(const glm::mat4& lightView,
		CameraComponent& camera,
		float& left, float& right, float& bottom, float& top,
		float& near_plane, float& far_plane);

	void RenderDepthMap(unsigned int depthMapArray, int screenWidth, int screenHeight);

	unsigned int getShadowMapArray() const { return shadowMapArray; }
private:
	unsigned int shader;
	unsigned int shadowShader;
	unsigned int depthMapShader;


	unsigned int shadowMapArray;
	unsigned int shadowFBO;

	unsigned int quadVAO, quadVBO;

	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

	


};

