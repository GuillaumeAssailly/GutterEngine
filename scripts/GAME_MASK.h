#include "EnvironmentSetup.h"

class PREPARATION_MASK : public ScriptManager::Mask {
public:
    PREPARATION_MASK() {
        addAction(new ScriptManager::Action("go_right",
            { GLFW_KEY_RIGHT },
            { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT },
            InputType::PRESS_ONE_TIME));
        addAction(new ScriptManager::Action("go_left",
            { GLFW_KEY_LEFT },
            { GLFW_GAMEPAD_BUTTON_DPAD_LEFT },
            InputType::PRESS_ONE_TIME));
        addAction(new ScriptManager::Action("turn_right",
            { GLFW_KEY_DOWN },
            { GLFW_GAMEPAD_BUTTON_DPAD_DOWN },
            InputType::PRESS_ONE_TIME));
        addAction(new ScriptManager::Action("turn_left",
            { GLFW_KEY_UP },
            { GLFW_GAMEPAD_BUTTON_DPAD_UP },
            InputType::PRESS_ONE_TIME));
        addAction(new ScriptManager::Action("launch",
            { GLFW_KEY_SPACE },
            { GLFW_GAMEPAD_BUTTON_A },
            InputType::PRESS_ONE_TIME));
    }
};