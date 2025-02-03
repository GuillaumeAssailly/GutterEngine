#include "system_manager.h"

SystemManager::SystemManager(GLFWwindow* window, unsigned int shader, unsigned int shadowShader, unsigned int depthMapDebugShader)
{
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
    renderSystem = new RenderSystem(shader, window);
    lightSystem = new LightSystem(shader);
    shadowSystem = new ShadowSystem(shader, shadowShader, depthMapDebugShader);
    lineSystem = new LineSystem();
}

SystemManager::~SystemManager()
{
    delete cameraSystem;
    delete motionSystem;
    delete renderSystem;
    delete lightSystem;
    delete lineSystem;
    delete shadowSystem;
}