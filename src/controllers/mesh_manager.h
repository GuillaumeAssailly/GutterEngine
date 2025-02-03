#pragma once
#include "../config.h"
#include "entity_manager.h"

class App;

class MeshManager
{
private:
    std::unordered_map<std::string, std::vector<physx::PxBase*>> physicsModels;
    std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> renderModels;
    std::unordered_map<std::string, unsigned int> texturesList;
    std::unordered_map<std::string, unsigned int> normalMapsList;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    EntityManager* entityManager;
public:
    MeshManager(EntityManager* em);
    ~MeshManager();

    friend class App;

    std::pair<unsigned int, unsigned int> make_cube_mesh(glm::vec3 size);
    std::pair<unsigned int, unsigned int>  make_model(const char*);
    unsigned int make_texture(const char* filename, const bool);
    void loadGLTF(const char* filePath, const char* texDir, const int);
};