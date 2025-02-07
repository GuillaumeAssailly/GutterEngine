#pragma once
#include "../config.h"
#include "entity_manager.h"

class App;
class UI;

class MeshManager
{
private:
    std::unordered_map<std::string, std::vector<physx::PxBase*>> physicsModels;
    std::unordered_map<std::string, std::vector<std::tuple<unsigned int, unsigned int, Material>>> renderList;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    EntityManager* entityManager;

public:
    MeshManager(EntityManager* em);
    ~MeshManager();

    friend class App;
    friend class UI;

    std::pair<unsigned int, unsigned int> make_cube_mesh(glm::vec3 size);
    std::pair<unsigned int, unsigned int>  make_model(const char*);
    unsigned int make_texture(const char* filename, const bool);
    void applyRenderModel(unsigned int entity, std::string model);
    void loadGLTF(const char* filePath, const char* texDir);
};