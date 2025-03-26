#include "CalculateScoreState.h"

void CalculateScoreState::displayScore(int nb_players)
{
    // En-tête du tableau
    std::cout << "\n\n\n---------------------------------------------------------------" << std::endl;
    std::cout << "|  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |   10  | Score" << std::endl;

    for (int p = 0; p < nb_players; p++) {
        int total_score = 0; // Score total du joueur
        std::cout << "---------------------------------------------------------------------" << std::endl;

        for (int i = 0; i < 10; i++) {
            std::cout << "| ";
            int frame_score = 0;

            // Gestion des lancers dans la frame (2 pour les frames 1-9, jusqu'à 3 pour la frame 10)
            int throws_in_frame = (i == 9) ? 3 : 2;

            for (int j = 0; j < throws_in_frame; j++) {
                int current_value = score_tab.at(p)[i][j];

                // Affichage des valeurs selon les règles
                if (current_value == -1) {
                    std::cout << " "; // Non joué
                }
                else if (current_value == 0) {
                    std::cout << "_"; // Gouttière
                }
                else if (current_value == 10) {
                    if (j == 0) {
                        std::cout << "X"; // Strike
                    }
                    else {
                        std::cout << "\\"; // Spare
                    }
                }
                else {
                    if (j == 1 && current_value + score_tab.at(p)[i][0] == 10) {
                        std::cout << "\\"; // Spare
                    }
                    else {
                        std::cout << current_value; // Lancer classique
                    }
                }

                // Calcul des scores
                if (current_value != -1) {
                    frame_score += current_value;

                    // Ajouter les bonus (gestion des frames précédentes)
                    if (i > 0 && j < 2) {
                        // Vérifie si la frame précédente est un Strike ou un Spare
                        if (score_tab.at(p)[i - 1][0] == 10) { // Strike précédent
                            total_score += current_value;
                        }
                        else if (score_tab.at(p)[i - 1][0] + score_tab.at(p)[i - 1][1] == 10 && j == 0) { // Spare précédent
                            total_score += current_value;
                        }
                    }

                    if (i > 1 && j == 0 && score_tab.at(p)[i - 2][0] == 10 && score_tab.at(p)[i - 1][0] == 10) {
                        // Bonus double Strike en cascade
                        total_score += current_value;
                    }
                }
                std::cout << " ";
            }

            // Mise à jour du score total pour la frame
            total_score += frame_score;
        }

        // Affichage du score total du joueur
        std::cout << "| " << total_score << std::endl;
    }

    std::cout << "---------------------------------------------------------------------" << std::endl;
}

void CalculateScoreState::onLoad() {

	for (int i = 1; i < 11; i++) {
		glm::quat rot = getRotationQuaternionByName("Pin" + std::to_string(i));
		if (rot.x > -0.05 && rot.x < 0.05 && rot.z > -0.05 && rot.z < 0.05) {
			non_modified_pin.push_back(i);
		}
	}

	score_tab.at(0)[current_turn][current_throw] = 10 - non_modified_pin.size();

	current_throw++;

	displayScore(1);

	setMainCameraByName("Camera2");
	changeState(AllStates::REPLAY_1);
}

void CalculateScoreState::running() {

}

void CalculateScoreState::onDestruct() {

}