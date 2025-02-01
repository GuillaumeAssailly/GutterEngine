#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {

    modelLocation = glGetUniformLocation(shader, "model");
	shadowMapLocation = glGetUniformLocation(shader, "shadowMap");
	reflectionTexLocation = glGetUniformLocation(shader, "reflectionTexture");
	lightSpaceMatrixLocation = glGetUniformLocation(shader, "lightSpaceMatrix");
    shaderProg = shader;
    this->window = window;
}

void RenderSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents,
    std::unordered_map<unsigned int, LightComponent>& lightComponents) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int lightCpt = 0;
     for (auto& lightEntity : lightComponents)
    {
        lightCpt++;
        if (lightEntity.second.type == POINT) continue;

		glActiveTexture(GL_TEXTURE4 + lightCpt);
		glBindTexture(GL_TEXTURE_2D, lightEntity.second.depthMap);
        // Pass the light space matrix to the shader
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightEntity.second.lightSpaceMatrix));
        glUniform1i(shadowMapLocation, 4 + lightCpt); // Set the shadow map to texture unit 4 + n

        ////Set shadowmap
        //std::string shadowMapsUniform = "shadowMaps[" + std::to_string(lightCpt-1) + "]";
        //GLint shadowMapsLoc = glGetUniformLocation(shaderProg, shadowMapsUniform.c_str());
        //glUniform1i(shadowMapsLoc, 4 + lightCpt);
    }

    for (std::pair<unsigned int, std::list<RenderComponent>> entity : renderComponents) {
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

            TransformComponent& transform = transformComponents[entity.first];

            // Set the model matrix based on the transform
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);

            // Convert the quaternion to a rotation matrix
            glm::mat4 rotationMatrix = glm::mat4_cast(transform.eulers);
            model *= rotationMatrix;

            glUniformMatrix4fv(
                modelLocation, 1, GL_FALSE,
                glm::value_ptr(model));

            // Bind the texture (material)
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, render.material);

            if (render.normalMap != 0) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, render.normalMap);
                glUniform1i(glGetUniformLocation(shaderProg, "normalMap"), 2);
                glUniform1i(glGetUniformLocation(shaderProg, "hasNormalMap"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProg, "hasNormalMap"), 0);

            }


            // Bind the vertex array (VAO)
            glBindVertexArray(render.mesh);

            glDrawElements(GL_TRIANGLES, render.indexCount, GL_UNSIGNED_INT, 0);
		}
        

    }

   
}