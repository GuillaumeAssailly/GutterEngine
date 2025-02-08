#pragma once
#include "../config.h"

#include "../systems/camera_system.h"
#include "../systems/motion_system.h"
#include "../systems/render_system.h"
#include "../systems/line_system.h"
#include "../systems/light_system.h"
#include "../systems/shadow_system.h"
#include "../systems/reflection_system.h"

class App;
class InputManager;
class UI;

class SystemManager
{
private:
    //Systems
    CameraSystem* cameraSystem;
    MotionSystem* motionSystem;
    RenderSystem* renderSystem;
    LightSystem* lightSystem;
    LineSystem* lineSystem;
    ShadowSystem* shadowSystem;
    ReflectionSystem* reflectionSystem;

public:
    SystemManager(GLFWwindow* window, unsigned int shader, unsigned int shadowShader, unsigned int depthMapDebugShader, unsigned int reflectionShader);
    ~SystemManager();

    friend class App;
    friend class InputManager;
    friend class UI;
};