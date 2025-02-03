#include "camera_system.h"

// Constructor for CameraSystem: Initializes with the shader program and GLFW window
CameraSystem::CameraSystem(unsigned int shader, GLFWwindow* window){
    this->window = window;

    glUseProgram(shader);
    viewLocation = glGetUniformLocation(shader, "view");
    projectionLocation = glGetUniformLocation(shader, "projection");
}

// Updates the camera system state, applying transformations and handling user inputs
bool CameraSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, CameraComponent>& cameraComponents,
    unsigned int cameraID, float dt
) {
    // Access the position and rotation of the current camera's transform
    glm::vec3& pos = transformComponents[cameraID].position;
    glm::quat& rotation = transformComponents[cameraID].eulers;

    // Retrieve the camera's properties
    CameraComponent camera = cameraComponents[cameraID];
    float fov = camera.fov;
    float aspectRatio = camera.aspectRatio;
    float farPlane = camera.farPlane;
    float nearPlane = camera.nearPlane;
    float sensitivity = camera.sensitivity;

    // Compute the view direction vectors based on the camera's rotation
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
    glm::vec3 forward = glm::normalize(rotationMatrix * camera.initialForward);
    glm::vec3 right = glm::normalize(rotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    glm::vec3 up = glm::normalize(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    glm::vec3 target = pos + forward;

    // Calculate the view matrix (camera orientation) and projection matrix (perspective transformation)
    viewMatrix = glm::lookAt(pos, target, up);
    projectionMatrix = glm::perspective(
        glm::radians(camera.fov), camera.aspectRatio, camera.nearPlane, camera.farPlane);

    // Send matrices to the shader
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Check if the ESC key is pressed to exit the update loop
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }

    // Handle right mouse button press for enabling/disabling movement
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

    // Handle camera movement and mouse rotation when in moving mode
    if (moving) {
        glm::vec3 dPos(0.0f);

        // Process keyboard inputs for movement along local axes
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dPos.x += 1.0f; // Forward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dPos.x -= 1.0f; // Backward
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dPos.y -= 1.0f; // Right
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dPos.y += 1.0f; // Left
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) dPos.z -= 1.0f; // Down
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) dPos.z += 1.0f; // Up

        // Normalize and scale movement by speed if any direction is active
        if (glm::length(dPos) > 0.1f) {
            dPos = glm::normalize(dPos);

            // Determine movement speed based on modifier keys
            float baseSpeed = 2.0f;
            float speedMultiplier = 1.0f;
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) speedMultiplier = 2.0f;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speedMultiplier = 0.5f;

            float speed = baseSpeed * speedMultiplier * dt;
            pos += speed * dPos.x * forward;
            pos += speed * dPos.y * right;
            pos += speed * dPos.z * up;
        }

        // Handle mouse input for camera rotation
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y); // Get current mouse position
        glfwSetCursorPos(window, mouse_x_ref, mouse_y_ref); // Reset cursor to reference point

        // Calculate yaw (horizontal) and pitch (vertical) rotations
        float yaw = sensitivity * static_cast<float>(mouse_x - mouse_x_ref) * dt;
        float pitch = -sensitivity * static_cast<float>(mouse_y - mouse_y_ref) * dt;

        // Apply rotations as quaternions
        glm::quat yawRotation = glm::angleAxis(-yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchRotation = glm::angleAxis(-pitch, glm::vec3(1.0f, 0.0f, 0.0f));

        // Update the camera's rotation by combining yaw and pitch
        rotation = glm::normalize(yawRotation * rotation * pitchRotation);
    }

    glfwPollEvents();
    return false;
}

glm::mat4 CameraSystem::GetViewMatrix() {
    return viewMatrix;
}

glm::mat4 CameraSystem::GetProjectionMatrix() {
    return projectionMatrix;
}
