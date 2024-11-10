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
    glm::vec3& eulers = transformComponents[cameraID].eulers;
    float theta = glm::radians(eulers.z);
    float phi = glm::radians(eulers.y);

    glm::vec3& right = cameraComponent.right;
    glm::vec3& up = cameraComponent.up;
    glm::vec3& forwards = cameraComponent.forwards;

    forwards = {
        glm::cos(theta) * glm::cos(phi),
        glm::sin(phi),  
        glm::sin(theta) * glm::cos(phi)
    };

    glm::vec3 global_up = { 0.0f, 1.0f, 0.0f };

    right = glm::normalize(glm::cross(forwards, global_up));

    up = glm::normalize(glm::cross(right, forwards));

    cameraComponent.viewMatrix = glm::lookAt(pos, pos + forwards, global_up);


    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(cameraComponent.viewMatrix));

    glm::vec3 dPos = { 0.0f, 0.0f, 0.0f };
	

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


    if (moving)
    {
        
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

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            return true;
        }

        //Mouse
        glm::vec3 dEulers = { 0.0f, 0.0f, 0.0f };


        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glfwSetCursorPos(window, mouse_x_ref, mouse_y_ref);
        

        dEulers.z = 0.1f * static_cast<float>(mouse_x - mouse_x_ref);
        dEulers.y = -0.1f * static_cast<float>(mouse_y - mouse_y_ref);

    eulers.y = glm::clamp(eulers.y + dEulers.y, -89.0f, 89.0f);

        eulers.z += dEulers.z;
        if (eulers.z > 360) {
            eulers.z -= 360;
        }
        else if (eulers.z < 0) {
            eulers.z += 360;
        }
    }

    glfwPollEvents();

    

    return false;
}