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
    glm::quat& rotation = transformComponents[cameraID].eulers; // Utilisation du quaternion

    glm::vec3& right = cameraComponent.right;
    glm::vec3& up = cameraComponent.up;
    glm::vec3& forwards = cameraComponent.forwards;

    // Convertir le quaternion en une matrice de rotation
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

    // Récupérer les vecteurs de direction à partir de la matrice de rotation
    forwards = glm::vec3(rotationMatrix[2]);  // Z est généralement l'axe avant
    up = glm::vec3(rotationMatrix[1]);        // Y est l'axe vers le haut
    right = glm::vec3(rotationMatrix[0]);     // X est l'axe vers la droite

    glm::mat4 view = glm::lookAt(pos, pos + forwards, up);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

    glm::vec3 dPos = { 0.0f, 0.0f, 0.0f };

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
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            dPos.x += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            dPos.y -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            dPos.x -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            dPos.y += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            dPos.z += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            dPos.z -= 1.0f;
        }

        float speed = 0.01f;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            speed *= 2.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 0.5f;
        }

        if (glm::length(dPos) > 0.1f) {
            dPos = glm::normalize(dPos);
            pos += speed * dPos.x * forwards;
            pos += speed * dPos.y * right;
            pos += speed * dPos.z * up;
        }

        // Gestion de la souris pour la rotation
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glfwSetCursorPos(window, mouse_x_ref, mouse_y_ref);

        float sensitivity = 0.005f; // Sensibilité de la souris
        float yaw = sensitivity * static_cast<float>(mouse_x - mouse_x_ref);
        float pitch = -sensitivity * static_cast<float>(mouse_y - mouse_y_ref);

        // Appliquer la rotation autour de l'axe Y (yaw) et X (pitch)
        glm::quat yawRotation = glm::angleAxis(-yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchRotation = glm::angleAxis(-pitch, right);

        // Mettre à jour la rotation de la caméra
        rotation = glm::normalize(pitchRotation * yawRotation * rotation);
    }

    glfwPollEvents();

    return false;
}