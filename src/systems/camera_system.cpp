#include "camera_system.h"

CameraSystem::CameraSystem(unsigned int shader, GLFWwindow* window) {
    this->window = window;

    glUseProgram(shader);
    viewLocation = glGetUniformLocation(shader, "view");
}

bool CameraSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    unsigned int cameraID, CameraComponent& cameraComponent, float dt) {

    glm::vec3& pos = transformComponents[cameraID].position;
    glm::quat& rotation = transformComponents[cameraID].eulers;

    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
    cameraComponent.forwards = glm::vec3(rotationMatrix[2]);
    cameraComponent.up = { 0,1,0 };
    cameraComponent.right = glm::vec3(rotationMatrix[0]);

    glm::mat4 view = glm::lookAt(pos, pos + cameraComponent.forwards, cameraComponent.up);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!moving) {
            moving = true;
            glfwGetCursorPos(window, &mouse_x_ref, &mouse_y_ref);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    else if (moving) {
        moving = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (moving) {
        glm::vec3 dPos(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dPos.x += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dPos.x -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dPos.y -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dPos.y += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) dPos.z += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) dPos.z -= 1.0f;

        if (glm::length(dPos) > 0.1f) {
            dPos = glm::normalize(dPos);

            float baseSpeed = 2.0f;
            float speedMultiplier = 1.0f;
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) speedMultiplier = 2.0f;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speedMultiplier = 0.5f;

            float speed = baseSpeed * speedMultiplier * dt;

            pos += speed * dPos.x * cameraComponent.forwards;
            pos += speed * dPos.y * cameraComponent.right;
            pos += speed * dPos.z * cameraComponent.up;
        }

        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glfwSetCursorPos(window, mouse_x_ref, mouse_y_ref);

        float sensitivity = 0.5f; 
        float yaw = sensitivity * static_cast<float>(mouse_x - mouse_x_ref) * dt;
        float pitch = -sensitivity * static_cast<float>(mouse_y - mouse_y_ref) * dt;

        glm::quat yawRotation = glm::angleAxis(-yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchRotation = glm::angleAxis(-pitch, cameraComponent.right);

        rotation = glm::normalize(pitchRotation * yawRotation * rotation);
    }

    glfwPollEvents();
    return false;
}