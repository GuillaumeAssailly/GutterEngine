#pragma once
#include "EnvironmentSetup.h"

class InitTurnState : public ScriptManager::StateOfGame {
public:
    InitTurnState(int id, int id_Mask, ScriptManager* sm) : StateOfGame(id, id_Mask, sm) {};
    void onLoad() override;
    void running() override;
    void onDestruct() override;
};
    