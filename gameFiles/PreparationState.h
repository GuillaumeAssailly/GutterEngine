#pragma once
#include "EnvironmentSetup.h"

class PreparationState : public GameManager::StateOfGame {
public:
    PreparationState(int id, int id_Mask, GameManager* sm) : StateOfGame(id, id_Mask, sm) { }
    void onLoad() override;
    void running() override;
    void onDestruct() override;

    bool init;
    float forceX;
    bool isPress;
};