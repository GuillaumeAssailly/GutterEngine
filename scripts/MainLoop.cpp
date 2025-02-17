#define MAINLOOP_CPP
#include "MainLoop.h"
#include "../src/controllers/script_manager.h"

MainLoop::MainLoop(ScriptManager* scriptManager) : scriptManager(scriptManager) {

	current_game_state = GameStateEnum::INIT_TURN;

}

void MainLoop::update_preparation_position_ball() {
	glm::vec3 look = getForwardMainCamera();
	glm::mat4 new_rotation = glm::rotate(glm::mat4(0.5f), -0.4f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 v = new_rotation * glm::vec4(look, 0);
	
	glm::vec3 position_camera = getPositionByName("Camera");

	glm::vec3 position_ball = { position_camera.x + v.x, position_camera.y - 0.15, position_camera.z + v.z };
	setPositionByName("Ball1", position_ball);
}


void MainLoop::mainLoop_one_frame()
{
	switch (current_game_state) {

		case GameStateEnum::INIT_TURN :
			vec3 initial_position = { WIDTH_CAMERA, HEIGHT_CAMERA, drawRandomFloat(MIN_POSITION_Z + 0.1, MAX_POSITION_Z - 0.1) };
			setPositionByName("Camera", initial_position);

			vec3 initial_angle = { 3.13f, drawRandomFloat(-1.671, -1.471), 3.16f };
			setRotationEulerByName("Camera", initial_angle);

			update_preparation_position_ball();

			current_game_state = GameStateEnum::PREPARATION;
			break;


		case GameStateEnum::PREPARATION:
			if (getInput_PressOneTime(GLFW_KEY_RIGHT)) {
				vec3 position = getPositionByName("Camera");
				if (position.z - 0.1 >= MIN_POSITION_Z) {
					position.z -= 0.1;
					setPositionByName("Camera", position);
					update_preparation_position_ball();
				}
			}
			if (getInput_PressOneTime(GLFW_KEY_LEFT)) {
				vec3 position = getPositionByName("Camera");
				if (position.z + 0.1 <= MAX_POSITION_Z) {
					position.z += 0.1;
					setPositionByName("Camera", position);
					update_preparation_position_ball();
				}
			}

			if (getInput_PressOneTime(GLFW_KEY_UP)) {
				glm::quat rotation_init = getRotationQuaternionByName("Camera");
				glm::quat rotation_angle = glm::angleAxis(PADDING_CAMERA_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat new_rotation = rotation_angle * rotation_init;
				if (new_rotation.w >= -VIEW_CAMERA_CENTER_Y - VIEW_CAMERA_LIMITS && new_rotation.y >= VIEW_CAMERA_CENTER_Y - VIEW_CAMERA_LIMITS) {
					setRotationQuaternionByName("Camera", new_rotation);
					update_preparation_position_ball();
				}
			}
			if (getInput_PressOneTime(GLFW_KEY_DOWN)) {
				glm::quat rotation_init = getRotationQuaternionByName("Camera");
				glm::quat rotation_angle = glm::angleAxis(-PADDING_CAMERA_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat new_rotation = rotation_angle * rotation_init;
				if (new_rotation.w <= -VIEW_CAMERA_CENTER_Y + VIEW_CAMERA_LIMITS && new_rotation.y <= VIEW_CAMERA_CENTER_Y + VIEW_CAMERA_LIMITS) {
					setRotationQuaternionByName("Camera", new_rotation);
					update_preparation_position_ball();
				}
			}

			if (getInput_PressOneTime(GLFW_KEY_SPACE)) {
				current_game_state = GameStateEnum::REPLAY_1;
				setMainCameraByName("Camera2");
			}
			break;


		case GameStateEnum::REPLAY_1:
			if (wait("replay1", 3.0) || getInput_PressOneTime(GLFW_KEY_SPACE)) {
				releaseTimer("replay1");
				current_game_state = GameStateEnum::INIT_TURN;
				setMainCameraByName("Camera");
			}
			break;

		default:
			break;
	}
}
