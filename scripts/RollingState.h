#pragma once
#include "EnvironmentSetup.h"

class RollingState : public ScriptManager::StateOfGame {
public:
    bool saved = false;

    RollingState(int id, int id_Mask, ScriptManager* sm) : StateOfGame(id, id_Mask, sm) {};
    void onLoad() override;
    void running() override;
    void onDestruct() override;
};