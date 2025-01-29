#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"
#include "../components/light_component.h"

class RenderSystem {
public:

    RenderSystem(unsigned int shader, GLFWwindow* window);

    void update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents,
        std::unordered_map<unsigned int, LightComponent>& lightComponents);

private:

    unsigned int modelLocation;
	unsigned int shadowMapLocation;
	unsigned int lightSpaceMatrixLocation;
	unsigned int shaderProg;
    GLFWwindow* window;
};