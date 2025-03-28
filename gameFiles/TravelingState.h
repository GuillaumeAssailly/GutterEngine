#pragma once
#include "EnvironmentSetup.h"

class TravelingState : public GameManager::StateOfGame {
public:
    TravelingState(int id, int id_Mask, GameManager* sm) : StateOfGame(id, id_Mask, sm) {};
    void onLoad() override;
    void running() override;
    void onDestruct() override;

    int compteur;
};