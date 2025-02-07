#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window, unsigned int shadowMapArray) {

    modelLocation = glGetUniformLocation(shader, "model");
	shadowMapLocation = glGetUniformLocation(shader, "shadowMap");
	reflectionTexLocation = glGetUniformLocation(shader, "reflectionTexture");
	lightSpaceMatrixLocation = glGetUniformLocation(shader, "lightSpaceMatrix");
    shaderProg = shader;
    this->window = window;
}

//Need to call this function after the init of every other system
void RenderSystem::Initialize(unsigned int shadowMapArray) {
    this->shadowMapArray = shadowMapArray;
}

void RenderSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents,
    std::unordered_map<unsigned int, LightComponent>& lightComponents)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProg);

    // Bind the shadow map array
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);
    glUniform1i(shadowMapLocation, 4); // Tell the shader the texture unit

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

			if (render.emissiveMap != 0) {
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, render.emissiveMap);
				glUniform1i(glGetUniformLocation(shaderProg, "emissiveMap"), 5);
				glUniform1i(glGetUniformLocation(shaderProg, "hasEmissive"), 1);
				glUniform1f(glGetUniformLocation(shaderProg, "emissiveForce"), render.emissiveForce);
			}
			else {
				glUniform1i(glGetUniformLocation(shaderProg, "hasEmissive"), 0);
			}

            if (render.aoMap != 0) {
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, render.aoMap);
				glUniform1i(glGetUniformLocation(shaderProg, "aoMap"), 6);
				glUniform1i(glGetUniformLocation(shaderProg, "hasAOMap"), 1);
            } 
            else {
				glUniform1i(glGetUniformLocation(shaderProg, "hasAOMap"), 0);
            }

            if (render.metalnessRoughnessMap != 0) {
                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D, render.metalnessRoughnessMap);
                glUniform1i(glGetUniformLocation(shaderProg, "metalRoughnessMap"), 7);
                glUniform1i(glGetUniformLocation(shaderProg, "hasMetalRoughnessMap"), 1);
            }
			else {
				glUniform1i(glGetUniformLocation(shaderProg, "hasMetalRoughnessMap"), 0);
			}

            // Draw the object
            glBindVertexArray(render.mesh);
            glDrawElements(GL_TRIANGLES, render.indexCount, GL_UNSIGNED_INT, 0);
        }
    }
}
