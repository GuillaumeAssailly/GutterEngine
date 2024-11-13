#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {

    modelLocation = glGetUniformLocation(shader, "model");
    this->window = window;
}

void RenderSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, RenderComponent>& renderComponents) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (std::pair<unsigned int, RenderComponent> entity : renderComponents) {
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
        glBindTexture(GL_TEXTURE_2D, entity.second.material);

        // Bind the vertex array (VAO)
        glBindVertexArray(entity.second.mesh);

        // Now we need to draw using indices. We assume that the VAO has the EBO already set up.
        // To render with the EBO, we use glDrawElements instead of glDrawArrays.
        glDrawElements(GL_TRIANGLES, entity.second.indexCount, GL_UNSIGNED_INT, 0); // Update this with the number of indices

    }

   
}