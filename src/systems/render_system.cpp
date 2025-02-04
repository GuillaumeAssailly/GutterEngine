#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window, unsigned int shadowMapArray) {

    modelLocation = glGetUniformLocation(shader, "model");
	shadowMapLocation = glGetUniformLocation(shader, "shadowMap");
	reflectionTexLocation = glGetUniformLocation(shader, "reflectionTexture");
	lightSpaceMatrixLocation = glGetUniformLocation(shader, "lightSpaceMatrix");
    shaderProg = shader;
	this->shadowMapArray = shadowMapArray;
    this->window = window;
}

void RenderSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents,
    std::unordered_map<unsigned int, LightComponent>& lightComponents)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProg);

    // Bind the shadow map array (only once)
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);
    glUniform1i(shadowMapLocation, 4); // Tell the shader the texture unit

    int lightIndex = 0;
    for (auto& lightEntity : lightComponents) {
        LightComponent& light = lightEntity.second;
        if (light.type == POINT) {
            lightIndex++;
            continue; // Skip point lights
        }

        // Pass the light space matrix to the shader
        std::string lightSpaceMatrixUniform = "lightSpaceMatrices[" + std::to_string(lightIndex) + "]";
        GLint lightSpaceMatrixLoc = glGetUniformLocation(shaderProg, lightSpaceMatrixUniform.c_str());
        glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));

        // Pass the shadow map layer index to the shader
        std::string shadowLayerUniform = "shadowMapLayers[" + std::to_string(lightIndex) + "]";
        GLint shadowLayerLoc = glGetUniformLocation(shaderProg, shadowLayerUniform.c_str());
        glUniform1i(shadowLayerLoc, light.shadowMapLayer);
        lightIndex++;

    }

    for (auto& entity : renderComponents) {
        TransformComponent& transform = transformComponents[entity.first];

        for (RenderComponent& render : entity.second) {
            if (render.isPlanarReflectable) {
                glUniform1i(glGetUniformLocation(shaderProg, "isPlanarReflectable"), 1);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, render.reflectionTexture);
                glUniform1i(reflectionTexLocation, 3);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProg, "isPlanarReflectable"), 0);
            }

            // Compute the model matrix
            glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position) * glm::mat4_cast(transform.eulers);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            // Bind material texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, render.material);

            // Bind normal map if available
            if (render.normalMap != 0) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, render.normalMap);
                glUniform1i(glGetUniformLocation(shaderProg, "normalMap"), 2);
                glUniform1i(glGetUniformLocation(shaderProg, "hasNormalMap"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProg, "hasNormalMap"), 0);
            }

            // Draw the object
            glBindVertexArray(render.mesh);
            glDrawElements(GL_TRIANGLES, render.indexCount, GL_UNSIGNED_INT, 0);
        }
    }
}
