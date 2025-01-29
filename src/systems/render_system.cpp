#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {

    modelLocation = glGetUniformLocation(shader, "model");
    shadowMapLocation = glGetUniformLocation(shader, "shadowMap");
    lightSpaceMatrixLocation = glGetUniformLocation(shader, "lightSpaceMatrix");
    shaderProg = shader;
    this->window = window;
}

void RenderSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, RenderComponent>& renderComponents,
    std::unordered_map<unsigned int, LightComponent>& lightComponents) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& lightEntity : lightComponents)
    {
        if (lightEntity.second.isDirectional == false) continue;
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lightEntity.second.depthMap);

        // Pass the light space matrix to the shader
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightEntity.second.lightSpaceMatrix));
        glUniform1i(shadowMapLocation, 1); // Set the shadow map to texture unit 1
    }

    for (std::pair<unsigned int, RenderComponent> entity : renderComponents) {
        TransformComponent& transform = transformComponents[entity.first];
        for (int i = 0; i < entity.second.meshes.size(); ++i) {
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
            glBindTexture(GL_TEXTURE_2D, entity.second.materials[i]);

            if (entity.second.normalMap[i] != 0) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, entity.second.normalMap[i]);
                glUniform1i(glGetUniformLocation(shaderProg, "normalMap"), 2);
                glUniform1i(glGetUniformLocation(shaderProg, "hasNormalMap"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProg, "hasNormalMap"), 0);

            }


            // Bind the vertex array (VAO)
            glBindVertexArray(entity.second.meshes[i]);

            glDrawElements(GL_TRIANGLES, entity.second.indexCount[i], GL_UNSIGNED_INT, 0);

        }

    }


}