#pragma once

#include "../loaders/json.hpp"
#include "saver.h"

using json = nlohmann::json;

class SaverJSON : public Saver
{
public:
    void saveEntities(App* app) override;
    SaverJSON(std::string);

private:
    std::string fileName;
    json jsonData;
    void saveQuilles(App* app, CameraComponent* camera) override;
    void savePinMaterial(App* app, int pinId);
    void saveBall(App* app, CameraComponent* camera) override;
    void saveLight(App* app, CameraComponent* camera) override;
    void saveCamera(App* app, CameraComponent* camera) override;
    void saveLane(App* app, CameraComponent* camera) override;

};
