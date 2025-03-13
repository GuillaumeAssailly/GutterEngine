#pragma once
#include "EnvironmentSetup.h"

class Replay1State : public ScriptManager::StateOfGame {
public:
    Replay1State(int id, int id_Mask, ScriptManager* sm) : StateOfGame(id, id_Mask, sm) { }
    void onLoad() override;
    void running() override;
    void onDestruct() override;
};