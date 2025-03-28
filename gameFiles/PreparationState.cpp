#include "PreparationState.h"


void PreparationState::onLoad()
{
	init = true;
	isPress = false;
	forceX = -15.0f;
}

void PreparationState::running() {
	if(init && wait("init", 0.001)) {
		update_preparation_position_ball(gameManager);
		init = false;
	}


	if (getAction("go_right") || getActionOnController("go_right", GLFW_JOYSTICK_1)) {
		vec3 position = getPositionByName("Camera");
		if (position.z - 0.1 >= MIN_POSITION_Z) {
			position.z -= 0.1;
			setPositionByName("Camera", position);
			update_preparation_position_ball(gameManager);
		}
	}
	if (getAction("go_left") || getActionOnController("go_left", GLFW_JOYSTICK_1)) {
		vec3 position = getPositionByName("Camera");
		if (position.z + 0.1 <= MAX_POSITION_Z) {
			position.z += 0.1;
			setPositionByName("Camera", position);
			update_preparation_position_ball(gameManager);
		}
	}

	if (getAction("turn_left") || getActionOnController("turn_left", GLFW_JOYSTICK_1)) {
		glm::quat rotation_init = getRotationQuaternionByName("Camera");
		glm::quat rotation_angle = glm::angleAxis(PADDING_CAMERA_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat new_rotation = rotation_angle * rotation_init;
		if (new_rotation.w >= -VIEW_CAMERA_CENTER_Y - VIEW_CAMERA_LIMITS && new_rotation.y >= VIEW_CAMERA_CENTER_Y - VIEW_CAMERA_LIMITS) {
			setRotationQuaternionByName("Camera", new_rotation);
			update_preparation_position_ball(gameManager);
		}
	}
	if (getAction("turn_right") || getActionOnController("turn_right", GLFW_JOYSTICK_1)) {
		glm::quat rotation_init = getRotationQuaternionByName("Camera");
		glm::quat rotation_angle = glm::angleAxis(-PADDING_CAMERA_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat new_rotation = rotation_angle * rotation_init;
		if (new_rotation.w <= -VIEW_CAMERA_CENTER_Y + VIEW_CAMERA_LIMITS && new_rotation.y <= VIEW_CAMERA_CENTER_Y + VIEW_CAMERA_LIMITS) {
			setRotationQuaternionByName("Camera", new_rotation);
			update_preparation_position_ball(gameManager);
		}
	}

	if (getAction("launch_press") || getActionOnController("launch_press", GLFW_JOYSTICK_1)) {
		isPress = true;
		if (wait("launch", 0.5)) {
			forceX -= 5.0f;
		}
	}

	if (forceX <= -30.0f || (isPress && getAction("launch_release")) || (isPress && getActionOnController("launch_release", GLFW_JOYSTICK_1)) ) {
		releaseTimer("launch");
		changeState(AllStates::ROLLING);

		enablePhysicByName("Ball1");
		glm::vec3 look = getForwardMainCamera();
		physx::PxVec3 force = { forceX, 0.4f, 0.0f + ((look.z < 0) ? look.z + 0.02f : look.z) * 3.0f * (-forceX / 4) };
		setForceByName("Ball1", force);
	}
}

void PreparationState::onDestruct()
{

}
