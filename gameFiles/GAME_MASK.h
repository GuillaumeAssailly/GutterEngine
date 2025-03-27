#include "EnvironmentSetup.h"

class PREPARATION_MASK : public GameManager::Mask {
public:
    PREPARATION_MASK() {
        addAction(new GameManager::Action("go_right",
            { GLFW_KEY_RIGHT },
            { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT },
            InputType::PRESS_ONE_TIME));
        addAction(new GameManager::Action("go_left",
            { GLFW_KEY_LEFT },
            { GLFW_GAMEPAD_BUTTON_DPAD_LEFT },
            InputType::PRESS_ONE_TIME));
        addAction(new GameManager::Action("turn_right",
            { GLFW_KEY_DOWN },
            { GLFW_GAMEPAD_BUTTON_DPAD_DOWN },
            InputType::PRESS_ONE_TIME));
        addAction(new GameManager::Action("turn_left",
            { GLFW_KEY_UP },
            { GLFW_GAMEPAD_BUTTON_DPAD_UP },
            InputType::PRESS_ONE_TIME));
        addAction(new GameManager::Action("launch_press",
            { GLFW_KEY_SPACE },
            { GLFW_GAMEPAD_BUTTON_A },
            InputType::PRESS));
        addAction(new GameManager::Action("launch_release",
            { GLFW_KEY_SPACE },
            { GLFW_GAMEPAD_BUTTON_A },
            InputType::RELEASE));
        addAction(new GameManager::Action("launch",
            { GLFW_KEY_SPACE },
            { GLFW_GAMEPAD_BUTTON_A },
            InputType::PRESS_ONE_TIME));
    }
};