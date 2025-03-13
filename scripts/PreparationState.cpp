#include "PreparationState.h"


void PreparationState::onLoad()
{

}

void PreparationState::running() {

	physx::PxVec3 force = {0.0f, 5.0f, 0.0f};
	//setForceByName("Ball1", force);

	if (getAction("go_right") || getActionOnController("go_right", GLFW_JOYSTICK_1)) {
		vec3 position = getPositionByName("Camera");
		if (position.z - 0.1 >= MIN_POSITION_Z) {
			position.z -= 0.1;
			setPositionByName("Camera", position);
			update_preparation_position_ball(scriptManager);
		}
	}
	if (getAction("go_left") || getActionOnController("go_left", GLFW_JOYSTICK_1)) {
		vec3 position = getPositionByName("Camera");
		if (position.z + 0.1 <= MAX_POSITION_Z) {
			position.z += 0.1;
			setPositionByName("Camera", position);
			update_preparation_position_ball(scriptManager);
		}
	}

	if (getAction("turn_left") || getActionOnController("turn_left", GLFW_JOYSTICK_1)) {
		glm::quat rotation_init = getRotationQuaternionByName("Camera");
		glm::quat rotation_angle = glm::angleAxis(PADDING_CAMERA_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat new_rotation = rotation_angle * rotation_init;
		if (new_rotation.w >= -VIEW_CAMERA_CENTER_Y - VIEW_CAMERA_LIMITS && new_rotation.y >= VIEW_CAMERA_CENTER_Y - VIEW_CAMERA_LIMITS) {
			setRotationQuaternionByName("Camera", new_rotation);
			update_preparation_position_ball(scriptManager);
		}
	}
	if (getAction("turn_right") || getActionOnController("turn_right", GLFW_JOYSTICK_1)) {
		glm::quat rotation_init = getRotationQuaternionByName("Camera");
		glm::quat rotation_angle = glm::angleAxis(-PADDING_CAMERA_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat new_rotation = rotation_angle * rotation_init;
		if (new_rotation.w <= -VIEW_CAMERA_CENTER_Y + VIEW_CAMERA_LIMITS && new_rotation.y <= VIEW_CAMERA_CENTER_Y + VIEW_CAMERA_LIMITS) {
			setRotationQuaternionByName("Camera", new_rotation);
			update_preparation_position_ball(scriptManager);
		}
	}

	if (getAction("launch") || getActionOnController("launch", GLFW_JOYSTICK_1)) {
		changeState(AllStates::REPLAY_1);
		setMainCameraByName("Camera2");
	}
}

void PreparationState::onDestruct()
{

}
