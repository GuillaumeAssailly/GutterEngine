#pragma once
#include "../config.h"

#include "../systems/camera_system.h"
#include "../systems/motion_system.h"
#include "../systems/render_system.h"
#include "../systems/line_system.h"
#include "../systems/light_system.h"
#include "../systems/shadow_system.h"

class App;

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

public:
    SystemManager(GLFWwindow* window, unsigned int shader, unsigned int shadowShader, unsigned int depthMapDebugShader);
    ~SystemManager();

    friend class App;
};