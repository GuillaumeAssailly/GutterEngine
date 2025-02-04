#pragma once
#include "../config.h"
#include "../components/camera_component.h"
#include "../components/physics_component.h"
#include "../components/static_physics_component.h"
#include "../components/transform_component.h"
#include "../components/light_component.h"
#include "../components/render_component.h"
class App;
class MeshManager;
class Inputmanager;
class UI;

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

    friend class App;
    friend class MeshManager;
    friend class InputManager;
    friend class UI;

    unsigned int make_entity(const std::string&);
    std::unordered_map<unsigned int, std::string>& getEntityNames();

    int cameraID = 0;

private:
    //Components
    std::unordered_map<unsigned int, std::string> entityNames;
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;
    std::unordered_map<unsigned int, StaticPhysicsComponent> staticPhysicsComponents;
    std::unordered_map<unsigned int, CameraComponent> cameraComponents;
    std::unordered_map<unsigned int, LightComponent> lightComponents;
    std::unordered_map<unsigned int, std::list<RenderComponent>> renderComponents;

    unsigned int entity_count = 0;
};