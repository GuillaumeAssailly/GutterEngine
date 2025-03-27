#pragma once
#include "EnvironmentSetup.h"

class Replay1State : public GameManager::StateOfGame {
public:
    Replay1State(int id, int id_Mask, GameManager* sm) : StateOfGame(id, id_Mask, sm) { }
    void onLoad() override;
    void running() override;
    void onDestruct() override;

    size_t replayIndex;
};