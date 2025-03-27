#include "system_manager.h"

SystemManager::SystemManager(GLFWwindow* window, unsigned int shader, unsigned int shadowShader, unsigned int depthMapDebugShader, unsigned int reflectionShader)
{
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
    shadowSystem = new ShadowSystem(shader, shadowShader, depthMapDebugShader);
    renderSystem = new RenderSystem(shader, window, shadowSystem->getShadowMapArray());
    lineSystem = new LineSystem();
    reflectionSystem = new ReflectionSystem(shader, reflectionShader);
    lightSystem = new LightSystem(shader, reflectionShader);
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