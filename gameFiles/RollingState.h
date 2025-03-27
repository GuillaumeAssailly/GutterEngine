#pragma once
#include "EnvironmentSetup.h"

class RollingState : public GameManager::StateOfGame {
public:
    bool saved = false;

    RollingState(int id, int id_Mask, GameManager* sm) : StateOfGame(id, id_Mask, sm) {};
    void onLoad() override;
    void running() override;
    void onDestruct() override;
};