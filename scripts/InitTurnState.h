#pragma once
#include "EnvironmentSetup.h"

class InitTurnState : public ScriptManager::StateOfGame {
public:
    InitTurnState(int id, int id_Mask, ScriptManager* sm) : StateOfGame(id, id_Mask, sm) {};
    void onLoad() override;
    void running() override;
    void onDestruct() override;

	glm::vec3 first_pin = { -22.0f, 0.01f, 6.0f };

	glm::vec3 offsets[10] = {
		{ -0.0f, 0.0f, 0.0f },
		{ -0.2635f, 0.0f, 0.1419f },
		{ -0.2635f, 0.0f, -0.1419f },
		{ -0.527f, 0.0f, -0.2838f },
		{ -0.527f, 0.0f, 0.0f },
		{ -0.527f, 0.0f, 0.2838f },
		{ -0.7905f, 0.0f, 0.1419f },
		{ -0.7905f, 0.0f, -0.1419f },
		{ -0.7905f, 0.0f, 0.4257f },
		{ -0.7905f, 0.0f, -0.4257f }
	};
};
    